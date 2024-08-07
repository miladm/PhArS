/*******************************************************************************
 *  dominator.cpp
 ******************************************************************************/

#include "dominator.h"


/* SETUP BASICBLOCK DOMINATOR SETS */
void build_dominators (List<basicblock*>* bbList) {
    /* FIND ENTRY POINTS */
	for (int i = 0; i < bbList->NumElements (); i++) {
		basicblock* n = bbList->Nth (i);
		if (n->getNumAncestors () == 0) {
			bool change = n->setDominators ();
			n->markAsEntryPoint ();
			n->setAllasDominators (false);
		} else {
			// bool change = n->setDominators (bbList); this approach is too mem expensive
			n->setAllasDominators (true);
			bool change = true;
		}
	}

    /* BUILD DOMINATOR */
	bool change = true;
	while (change) {
		change = false;
		for (int i = 0; i < bbList->NumElements (); i++) {
			basicblock* n = bbList->Nth (i);
			if (n->isEntryPoint ()) continue;
			int indx;
			bool foundFirstAncestor = false;
			map<ADDR,basicblock*> intersection;

            /* FIND AN ENTRY POINT THAT IS NOT DUE TO A LOOP (THIS BLOCK AVOIDS
             * VOID INTERSECTION RESULT LATER) */
            for (indx = 0; indx < n->getNumAncestors (); indx++) {
				if (n->getNthAncestor (indx)->getAllasDominators () == true) continue;
				intersection = n->getNthAncestor (indx)->getDominators ();
				foundFirstAncestor = true;
				break;
			}

            /* DO THE INTERSECT OF DOMINATOR SET OF ALL TRUE DOMINATORS */
			if (foundFirstAncestor == true) {
                for (int j = indx + 1; j < n->getNumAncestors (); j++) {
                    if (n->getNthAncestor (j)->getAllasDominators () == true) continue;
                    map<ADDR,basicblock*> out, nthAncestorDom;
                    nthAncestorDom = n->getNthAncestor (j)->getDominators ();
                    set_intersection (intersection.begin (), intersection.end (),
                                      nthAncestorDom.begin (), nthAncestorDom.end (),
                                      std::inserter (out, out.begin ()));
                    intersection = out;
                }
				if (change == false) change = n->setDominators (intersection);
				else 			  bool temp = n->setDominators (intersection);
			    n->setAllasDominators (false);
			} else {
				if (change == false) change = n->setDominators ();
				else		      bool temp = n->setDominators ();
			}
		}
	}

	#ifdef DEBUG_DOM
	for (int i = 0; i < bbList->NumElements (); i++) {
		map<ADDR,basicblock*> dom = bbList->Nth (i)->getDominators ();
		printf ("dom of %llx: ",bbList->Nth (i)->getID ());
		for (map<ADDR,basicblock*>::iterator it = dom.begin (); it != dom.end (); it++) {
			printf ("%llx, ",it->first);
		}
		printf ("\n");
	}
	#endif
}

void build_strict_dominators (List<basicblock*>* bbList) {
	for (int i = 0; i < bbList->NumElements (); i++)
		bbList->Nth (i)->buildSDominators ();

	#ifdef DEBUG_DOM
	for (int i = 0; i < bbList->NumElements (); i++) {
		map<ADDR,basicblock*> dom = bbList->Nth (i)->getDominators ();
		printf ("Strict dom of %llx: ",bbList->Nth (i)->getID ());
		for (map<ADDR,basicblock*>::iterator it = dom.begin (); it != dom.end (); it++) {
			if (bbList->Nth (i)->isASDominator (it->first)) printf ("%llx, ",it->first);
		}
		printf ("\n");
	}
	#endif
}

/* 
 * SETUP DOMINATOR TREE 
 * THIS PASS REQUIRES build_dominators () TO RUN FIRST
 */
void build_idom (List<basicblock*>* bbList) {
	for (int i = 0; i < bbList->NumElements (); i++)
		bbList->Nth (i)->buildImmediateDominators ();
}

/* SETUP DOMINATOR TREE */
void build_dominator_tree (List<basicblock*>* bbList) {
	for (int i = 0; i < bbList->NumElements (); i++)
		bbList->Nth (i)->buildDomTree ();
	#ifdef DEBUG_DOM
	for (int i = 0; i < bbList->NumElements (); i++) {
		map<ADDR,basicblock*> dom = bbList->Nth (i)->getDominators ();
		printf ("idom of %llx: ",bbList->Nth (i)->getID ());
		for (map<ADDR,basicblock*>::iterator it = dom.begin (); it != dom.end (); it++) {
			if (bbList->Nth (i)->isInIDom (it->first)) printf ("%llx, ",it->first);
		}
		printf ("\n");
	}
	#endif
}

static void findDomEntryPoints (List<basicblock*> *bbList, List<basicblock*> *interiorBB) {
	for (int i = 0; i < bbList->NumElements (); i++) {	
		basicblock* bb = bbList->Nth (i);
		if (bb->getSDominatorSize () == 0) {
			interiorBB->Append (bb);
		}
	}
}

/* FIND THE DOMINANCE FRONTIER OF ONE BB */
void findDF (basicblock* bb) {
    Assert (!bb->isVisited () && "An attempt to revisit a block in findDF");
	bb->setAsVisited ();

    map<ADDR,basicblock*> children = bb->getChildren ();
    map<ADDR,basicblock*>::iterator it1, it2;

    /* VISIT DOMINANCE TREE CHILDREN */
    bb->resetDF ();
    for (it1 = children.begin (); it1 != children.end (); it1++) {
        basicblock *child = it1->second;
        findDF (child);
        map<ADDR,basicblock*> childDF = child->getDF ();
        for (it2 = childDF.begin (); it2 != childDF.end (); it2++) {
            basicblock* u = it2->second;
            if (u->isASDominator (bb->getID ()) == false)
                bb->addToDFset (u);
        }
    }

    /* VISIT CFG DESCENDENTS */
    for (int i = 0; i < bb->getNumDescendents (); i++) {
        basicblock* descendent = bb->getNthDescendent (i);
        if (descendent->isASDominator (bb->getID ()) == false)
            bb->addToDFset (descendent);
    }
}

void build_dominance_frontier (List<basicblock*>* bbList) {
	List<basicblock*> *interiorBB = new List<basicblock*>;
	findDomEntryPoints (bbList, interiorBB);

	for (int i = 0; i < bbList->NumElements (); i++)
		bbList->Nth (i)->setAsUnvisited ();
	for (int i = 0; i < interiorBB->NumElements (); i++) {
		basicblock *bb = interiorBB->Nth (i);
        findDF (bb);
    }
	for (int i = 0; i < bbList->NumElements (); i++) {
		if (!bbList->Nth (i)->isVisited ()) 
            printf ("Unvisited BB ID: %llx,%d,%d\n", bbList->Nth (i)->getID (), 
                                                     bbList->Nth (i)->getNumAncestors (), 
                                                     bbList->Nth (i)->numNonBackEdgeAncestors ());
		bbList->Nth (i)->setAsUnvisited ();
	}
    delete interiorBB;
}

void setup_dominance_frontier (List<basicblock*>* bbList) {
	printf ("\tBuild Dominators\n");
	build_dominators (bbList);
	printf ("\tBuild Strict Dominators\n");
	build_strict_dominators (bbList);
	printf ("\tBuild Immediate Dominators\n");
	build_idom (bbList);
	printf ("\tBuild Dominance Tree\n");
	build_dominator_tree (bbList);
	printf ("\tBuild Dominance Frontier\n");
	build_dominance_frontier (bbList);
}
