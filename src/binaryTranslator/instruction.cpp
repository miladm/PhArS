/*******************************************************************************
 *  instruction.cpp
 ******************************************************************************/

#include "utility.h"
#include "global.h"
#include "instruction.h"
#include "dependencyTable.h"

long int nextRenReg = INIT_RENAME_REG_NUM;

instruction::instruction() {
	_brDst = -1;
	_insAddr = -1;
	_memSize = 0;
	_memWrite = false;
	_memRead = false;
	_latency = -1;
	_insType = 'x'; //Place holder
	_brBias = -1.0; //Place holder
	_bpAccuracy = -1.0; //Place holder
	_missRate = 0.0;
	_longestPath = -1;
	_r_read  = new List<long int>;
	_r_write = new List<long int>;
	_r_write_old = new List<long int>;
	_r  = new List<long int>;
	_r_allocated = new List<long int>;
	_rt = new List<int>;
	_rk = new List<regKind>;
	_ancestors   = new List<instruction*>;
	_regAncestors   = new List<instruction*>;
	_dependents  = new List<instruction*>;
}

instruction::~instruction() {
	delete _r_read;
	delete _r_write;
	delete _r_write_old;
	delete _r;
	delete _r_allocated;
	delete _rt;
	delete _rk;
	delete _ancestors;
	delete _dependents;
	delete _regAncestors;
}

void instruction::setOpCode(const char *opCode) {strcpy(_opCode, opCode);}

void instruction::setInsAddr(ADDR insAddr) {
	Assert(insAddr > 0 && "insAddr must be larger than zero.");
	_insAddr = insAddr;
}

void instruction::setBrDst(ADDR brDst) {
	Assert(brDst >= 0 && "brDst must be larger than zero.");
	_brDst = brDst;
}

void instruction::setInsAsm(const char *command) {strcpy(_command, command);}

void instruction::setType(const char insType) {
	Assert((insType == 'j' || insType == 'o' || insType == 'c' || 
	        insType == 'r' || insType == 'b' || insType == 'M') && "insType value is not recognized.");
	if (_insType != 'x') return; //Do not set type more than once
	Assert(_latency == -1 && "Invalid latency value detected.");
	_insType = insType;
	if (getType() == 'b') {
		_brBias = 0.5;
		_bpAccuracy = 0.5;
		_latency = BR_LATENCY;
	} else if (getType()  == 'c' || getType()  == 'j') {
		_brBias = 1.0;
		_bpAccuracy = 1.0;
		_latency = ALU_LATENCY;
	} else if (getType() == 'M') {
		_brBias = 0.0;
		_bpAccuracy = 1.0;
		_latency = L1_LATENCY;
	} else { //NOTE: in this not wrong to do? covers fall through paths that are not all branch ops
		_brBias = 0.0;
		_bpAccuracy = 1.0;
		_latency = ALU_LATENCY;
	}
}

void instruction::setBrTakenBias(double brBias) {
	Assert((brBias >= 0.0 && brBias <= 1.0) && "brBias value is not recognized.");
	_brBias = brBias;
}

void instruction::setBPaccuracy(double bpAccuracy) {
	Assert((bpAccuracy >= 0.0 && bpAccuracy <= 1.0) && "bpAccuracy value is not recognized.");
	_bpAccuracy = bpAccuracy;
}

const char *instruction::getOpCode() {return _opCode;}

ADDR instruction::getInsAddr() {
	Assert(_insAddr > 0 && "insAddr must be larger than zero.");
	return _insAddr;
}

ADDR instruction::getBrDst() {
	//Assert(_brDst >= 0 && "brDst must be larger than zero.");
	return _brDst;
}

const char *instruction::getInsAsm() {return _command;}

const char instruction::getType() {
	Assert((_insType == 'j' || _insType == 'o' || _insType == 'c' || 
	        _insType == 'r' || _insType == 'b' || _insType == 'M') && "insType value is invalid.");
	return _insType;
}

double instruction::getBrTakenBias() {
	Assert((_brBias >= 0.0 && _brBias <= 1.0) && "_brBias value is not recognized.");
	return _brBias;
}

double instruction::getBPaccuracy() {
	Assert((_bpAccuracy >= 0.0 && _bpAccuracy <= 1.0) && "_bpAccuracy value is not recognized.");
	return _bpAccuracy;
}

void instruction::setLdMissRate(double missRate) {
	Assert(missRate >= 0 && missRate <= 1 && "Invalid miss rate fetched\n");
	_missRate = missRate;

	Assert(_latency != -1 && "Invalid latency value detected.");
	if (_missRate > UPLD_THRESHOLD) _latency = L2_LATENCY;
}

double instruction::getLdMissRate() {
	Assert(_missRate >= 0 && _missRate <= 1 && "Invalid miss rate value\n");
	return _missRate;
}

void instruction::setRegister (long int *r, int *rt) {
    long int tempR = *r;
    int tempRT = *rt; 
	if (!(tempR <= X86_REG_HI && tempR >= X86_REG_LO)) printf("invalid reg: %d\n",tempR);
    //Assert(tempR >= 1); //TODO this is remved to enable instruction injection
    Assert(tempRT >= READ && tempRT <= WRITE);
	Assert(tempR <= X86_REG_HI && tempR >= X86_REG_LO && "Invalid register value");
    (_r)->Append(tempR);
    (_rt)->Append(tempRT);
	if (tempRT == READ) {
		(_r_read)->Append(tempR);
	} else if (tempRT == WRITE) {
		(_r_write)->Append(tempR);
		(_r_write_old)->Append(tempR);
	}
}

void instruction::setReadVar(int var, int subscript) {
	Assert((var <= X86_REG_HI && var >= X86_REG_LO) && "Invalid architectural register assignment.");
	Assert((subscript >= 0 || subscript == -2) && "Invalid SSA register assignment value");
	// printf("ins (%llx): %d_%d\n", getInsAddr(),var,subscript);
	if (_readVar.find(var) != _readVar.end() && subscript != _readVar[var]) 
		printf("\tWARNING: Rewriting an already assigned READ reg (old:%d,new:%d)\n", subscript,_readVar[var]);
	if (_readVar.find(var) != _readVar.end() && subscript!=-2 && _readVar[var]==-2)
		_readVar.erase(var);
	_readVar.insert(pair<int,int>(var,subscript));
}

void instruction::setWriteVar(int var, int subscript) {
	Assert((var <= X86_REG_HI && var >= X86_REG_LO) && "Invalid architectural register assignment.");
	Assert(subscript >= 0 && "Invalid SSA register assignment value");
	if (_writeVar.find(var) != _writeVar.end()) printf("trying to rewrite an already assigned write register(old:%d,new:%d)\n", subscript,_writeVar[var]);
	if (_writeVar.find(var) != _writeVar.end() && subscript>0 && _writeVar[var]==0)
		_writeVar.erase(var);
	_writeVar.insert(pair<int,int>(var,subscript));
}

void instruction::setArchReg(long int r) {
	if (!((r <= GRF_HI && r >= GRF_LO) || (r <= LRF_HI && r >= LRF_LO))) printf("invalid arch register value: %d\n",r);
	Assert(((r <= GRF_HI && r >= GRF_LO) || (r <= LRF_HI && r >= LRF_LO)) && "Invalid architectural register assignment.");
	Assert(_r_allocated->NumElements() <= _r->NumElements());
	_r_allocated->Append(r);
}

long int instruction::getNthArchReg(int indx) {
	Assert(indx < _r_allocated->NumElements() && indx >= 0 && "Out of range architectural register access.");
	return _r_allocated->Nth(indx);
}

bool instruction::isAlreadyAssignedArcRegs() {
	if (_r_allocated->NumElements() == _r->NumElements()) return true;
	else return false;
}

void instruction::removeNthRegister(int i) { 
    Assert(i < _r->NumElements() && i >= 0 && "Invalid index to access instruction registers");
    _r->RemoveAt(i);
    _rt->RemoveAt(i);
}

long int instruction::getNthReg(int i) { 
    Assert(i < _r->NumElements() && i >= 0);
    return _r->Nth(i);
}

long int instruction::getNthOldWriteReg(int i) {
    Assert(i < _r_write_old->NumElements() && i >= 0);
    return _r_write_old->Nth(i);
}

long int instruction::getNthReadReg(int i) { 
    Assert(i < _r_read->NumElements() && i >= 0);
    return _r_read->Nth(i);
}

long int instruction::getNthWriteReg(int i) { 
    Assert(i < _r_write->NumElements() && i >= 0);
    return _r_write->Nth(i);
}

long int instruction::getReadRegSubscript(long int var) { 
	// return _readVar.size();
	if (_readVar.find(var) != _readVar.end())
		return _readVar[var];
	else
		return -1;
}

long int instruction::getWriteRegSubscript(long int var) { 
	// return _writeVar.size();
	if (_writeVar.find(var) != _writeVar.end())
		return _writeVar[var];
	else
		return -1;
}

int instruction::getNthRegType(int i) { 
    Assert(i < _r->NumElements() && i >= 0);
    return _rt->Nth(i);
}

int instruction::getNumReg() {
    Assert (_r->NumElements() == _rt->NumElements() && "Number of registers and reg-types don't match");
    return _r->NumElements();
}

int instruction::getNumReadReg() {
    return _r_read->NumElements();
}

int instruction::getNumWriteReg() {
    return _r_write->NumElements();
}

/* replace each register with its corresponding SSA value */ 
void instruction::makeUniqueRegs() {
	for (int i = 0; i < _r->NumElements(); i++) {
		long int reg = _r->Nth(i);
		int regT = _rt->Nth(i);
		long int ssa;
		//printf("%d,%d,%d,%llx\n",regT,_readVar.find(reg) != _readVar.end(),_writeVar.find(reg) != _writeVar.end(), getInsAddr());
		if (regT == READ) {
			Assert(_readVar.find(reg) != _readVar.end());
			ssa = _readVar[reg]*100+reg;
		} else if (regT == WRITE) {
			Assert(_writeVar.find(reg) != _writeVar.end());
			ssa = _writeVar[reg]*100+reg;
		} else {
			Assert( true == false && "Invalid register type.");
		}
		_r->RemoveAt(i);
		_r->InsertAt(ssa,i);
	}
}

//This code is used when there is no RF in the simulator
std::string instruction::getRegisterStr() {
	std::stringstream ss;
	std::string s;
	if (getNumReg() == 0) {
		ss << "\n";
	} else {
		for (int i = 0; i < getNumReg(); i++) {
			if (getNthRegType(i) == WRITE && getRenamedReg(getNthReg(i)) != -1)
				ss << getRenamedReg(getNthReg(i)) << "#" << getNthRegType(i) << ",";
			else
				ss << getNthReg(i) << "#" << getNthRegType(i) << ",";
		}
		ss << "\n";
	}
	s = ss.str();
	return s;
}

std::string instruction::getArchRegisterStr() {
	std::stringstream ss;
	std::string s;
	if (getNumReg() == 0) {
		ss << "\n";
	} else {
		for (int i = 0; i < getNumReg(); i++) {
			ss << getNthArchReg(i) << "#" << getNthRegType(i) << ",";
		}
		ss << "\n";
	}
	s = ss.str();
	return s;
}


int instruction::getLatency() {
    return _latency;
}

void instruction::setWrMemType() {
	Assert(_memWrite == false && "invalid _memWrite value");
	_memWrite = true;
}

bool instruction::isWrMemType() {
	return _memWrite;
}

void instruction::setRdMemType() {
	Assert(_memRead == false && "invalid _memWrite value");
	_memRead = true;	
}

bool instruction::isRdMemType() {
	return _memRead;
}

void instruction::setMemAccessSize(int memSize) { //in bytes
	Assert(memSize > 0 && "invalid memSize value");
	// Assert((memSize == 1 || memSize == 2 || memSize == 4 || memSize == 8 || memSize == 16 || memSize == 32) && "invalid memSize value");
	_memSize = memSize; //TODO this way of assigning memory sizes is wrong. you should do this based on dynamic execution informaiton
}

int instruction::getMemAccessSize() { //in bytes
	Assert(_memSize > 0 && "invalid _memSize value");
	// Assert((_memSize == 1 || _memSize == 2 || _memSize == 4 || _memSize == 8 || _memSize == 16 || _memSize == 32) && "invalid _memSize value");
	return _memSize;
}

void instruction::setAsDependent(instruction* ins) {
	_dependents->Append(ins);
}

void instruction::setAsAncestor(instruction* ins) {
	if (ins->getInsAddr() > getInsAddr()) printf("\tAncestor Address %llx > Descendent Address %llx (%s, line: %d)\n", ins->getInsAddr(), getInsAddr(), __FILE__, __LINE__);
	_ancestors->Append(ins);
}

void instruction::setAsRegAncestor(instruction* ins) {
	if (ins->getInsAddr() > getInsAddr()) printf("\tAncestor Address %llx > Descendent Address %llx (%s, line: %d)\n", ins->getInsAddr(), getInsAddr(), __FILE__, __LINE__);
	_regAncestors->Append(ins);
}

bool instruction::isInsRepeated(instruction* ins, List<instruction*>*_ancestors) {
	Assert (ins != NULL);
	for (int i = 0; i < _ancestors->NumElements(); i++)
		if (_ancestors->Nth(i)->getInsAddr() == ins->getInsAddr()) {
			return true;
		}
	return false;
}

void instruction::dependencyTableCheck (dependencyTable *depTables) {
	//Register true dependency check for all instruction types
	for (int i = 0; i < _r->NumElements(); i++) {
		if (_rt->Nth(i) == READ) { //TODO Does this line make sense? (int vs. memType)
			instruction *temp = depTables->regLookup(_r->Nth(i),REG_WRITE);//RAW
			if (temp != NULL) {
				if (isInsRepeated(temp,_ancestors)==false) {
					temp->setAsDependent(this);
					setAsAncestor(temp);
					setAsRegAncestor(temp);
				}
				long int renReg = temp->getRenamedReg(_r->Nth(i));
				renameReadReg(i,renReg);
			}
		// } else if (_rt->Nth(i) == WRITE) {
		// 	instruction *temp = depTables->regLookup(_r->Nth(i),REG_READ);//WAR
		// 	if (temp != NULL && isInsRepeated(temp,_ancestors)==false) {
		// 		temp->setAsDependent(this);
		// 		setAsAncestor(temp);
		// 	}
		// 	temp = depTables->regLookup(_r->Nth(i),REG_WRITE);//WAW
		// 	if (temp != NULL && isInsRepeated(temp,_ancestors)==false) {
		// 		temp->setAsDependent(this);
		// 		setAsAncestor(temp);
		// 	}
		}
	}
	//Memory dependency (no memory disambiguation)
	List<instruction*>* stList = depTables->wrLookup();
	if (getType() == 'M' && stList->NumElements() > 0) {
		instruction *storeOp = stList->Nth(stList->NumElements()-1);
		storeOp->setAsDependent(this);
		setAsAncestor(storeOp);
	}
	if (getType() == 'M' && isWrMemType() == true) {
		depTables->addWr(this);
	}
	//Update write register table (must be done last to avoid deadlock/wrong dependency)
	for (int i = 0; i < _r->NumElements(); i++) {
		if (_rt->Nth(i) == WRITE) {
			depTables->addReg(i, _r->Nth(i), this, REG_WRITE); //overwrites existing table entry for reg
		// } else if (coreType == NO_CORE && _rt->Nth(i) == READ) { //TODO register renaming breaks this block of code
		} else if (_rt->Nth(i) == READ) { //TODO register renaming breaks this block of code
			depTables->addReg(i, _r->Nth(i), this, REG_READ); //overwrites existing table entry for reg
		}
	}
}

void instruction::setRdAddrSet(set<ADDR> &addrSet) {
	_memRdAddr = addrSet;
	printf("debug: read set of ins %llx: %d\n", getInsAddr(), _memRdAddr.size());
}

void instruction::setWrAddrSet(set<ADDR> &addrSet) {
	_memWrAddr = addrSet;
	printf("debug: write set of ins %llx: %d\n", getInsAddr(), _memWrAddr.size());
}

int instruction::getNumAncestors() {
	return _ancestors->NumElements();
}

int instruction::getNumDependents() {
	return _dependents->NumElements();
}

List<instruction*>* instruction::getDependents() {
	return _dependents;
}

List<instruction*>* instruction::getAncestors() {
	return _ancestors;
}

List<instruction*>* instruction::getRegAncestors() {
	return _regAncestors;
}

void instruction::resetLongestPath() {
	_longestPath = -1;
}

void instruction::setLongestPath(int longestPath) {
	Assert(_longestPath == -1 && longestPath > 0 && "Invalid longest path value.");
	_longestPath = longestPath;
}

bool instruction::isLongestPathSet() {
	if (_longestPath == -1) return false;
	else return true;
}

int instruction::getLongestPath() {
	Assert(_longestPath > 0 && "Invalid longest path value.");
	return _longestPath;
}

void instruction::setMy_BBorPB_id (ADDR id) {
	Assert(id > 0 && "BB or PB id is invalid.");
	_myBBs.insert(id);
}

ADDR instruction::getMy_BB_id () {
	Assert(_myBBs.size() == 1 && "Instruction belongs to too many BB's.");
	set<ADDR>::iterator it = _myBBs.begin();
	return *it;
}

ADDR instruction::getMy_first_PB_id() {
	set<ADDR>::iterator it = _myBBs.begin();
	return *it;
}

set<ADDR> instruction::getMy_PB_id () {
	Assert(_myBBs.size() > 0 && "Instruction must belong to a BB.");
	return _myBBs;
}

void instruction::renameWriteReg(int indx, long int reg) {
    nextRenReg++;
    Assert(reg <= NUM_REGISTERS && "Invalid Register Number");
    Assert(nextRenReg > INIT_RENAME_REG_NUM && "Invalid Rename Register Number");
	if (writeRegRenMap.count(reg) > 0)
		writeRegRenMap.erase(reg);
    _r->RemoveAt(indx);
    _r->InsertAt(nextRenReg,indx);
    writeRegRenMap.insert(pair<long int,long int>(reg,nextRenReg));
}

void instruction::renameReadReg(int indx, long int renReg) {
    Assert(_rt->Nth(indx) == READ);
    int num1 = _r->NumElements();
    if (renReg != -1) {
        _r->RemoveAt(indx);
        _r->InsertAt(renReg,indx);
    }
    int num2 = _r->NumElements();
    Assert (num1 == num2);
}

long int instruction::getRenamedReg(long int reg) {
    if (writeRegRenMap.count(reg) > 0)
        return writeRegRenMap.find(reg)->second;
    else
        return -1; //reg is not returned
}

void instruction::allocatedRegister(long int r_allocated, regKind rk) {
	Assert((rk == LRF || rk == GRF) && "Register Kind is undefined");
	Assert(((r_allocated >= LRF_LO && r_allocated <= LRF_HI) ||
           (r_allocated >= GRF_LO && r_allocated <= GRF_HI)) &&
           "Out of bound register allocation");
	_r_allocated->Append(r_allocated);
	_rk->Append(rk);
}