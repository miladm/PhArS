/*
 * Copyright (c) 2004-2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Kevin Lim
 */

#ifndef __CPU_O3_BPRED_UNIT_HH__
#define __CPU_O3_BPRED_UNIT_HH__

#include <list>

#include "base/statistics.hh"
#include "base/types.hh"
#include "cpu/pred/2bit_local.hh"
#include "cpu/pred/btb.hh"
#include "cpu/pred/ras.hh"
#include "cpu/pred/hybrid.hh"
#include "cpu/pred/hybrid_skew.hh"
#include "cpu/inst_seq.hh"

struct DerivO3CPUParams;

/**
 * BASICALLY A WRAPPER CLASS TO HOLD BOTH THE BRANCH PREDICTOR
 * AND THE BTB.
 */

class BPredUnit
{
  private:
    typedef typename Impl::DynInstPtr DynInstPtr;

    enum PredType {
        Local,
        Tournament,
		Hybrid,
		HybridSkew
    };

    PredType predictor;

    const std::string _name;
  public:

    /**
     * @param params The params object, that has the size of the BP and BTB.
     */
    BPredUnit(DerivO3CPUParams *params);

    const std::string &name() const { return _name; }

    /**
     * Registers statistics.
     */
    void regStats();

    void switchOut();

    void takeOverFrom();

    /**
     * Predicts whether or not the instruction is a taken branch, and the
     * target of the branch if it is taken.
     * @param inst The branch instruction.
     * @param PC The predicted PC is passed back through this parameter.
     * @param tid The thread id.
     * @return Returns if the branch is taken or not.
     */
    bool predict(DynInstPtr &inst, TheISA::PCState &pc, ThreadID tid, bool odbpStatic, bool predTaken, int positionInFetchGroup, int positionInLine, bool &allPredStatic, int oracleBP, bool onlyLP);
    void increase_BP_lookup(int);
    void increase_BP_lookup_and_update(int);
    void increase_BTB_lookup();


    /* only update the prediction history map */
    //void updatePredHist(bool predict, ThreadID tid);

    // @todo: Rename this function.
    void BPUncond(void * &bp_history);
    //Song. similar to above
    void BP_onDemand(void * &bp_history, bool taken);
    /**
     * Tells the branch predictor to commit any updates until the given
     * sequence number.
     * @param done_sn The sequence number to commit any older updates up until.
     * @param tid The thread id.
     */
    void update(const InstSeqNum &done_sn, ThreadID tid);

    /**
     * Squashes all outstanding updates until a given sequence number.
     * @param squashed_sn The sequence number to squash any younger updates up
     * until.
     * @param tid The thread id.
     */
    void squash(const InstSeqNum &squashed_sn, ThreadID tid);

    /**
     * Squashes all outstanding updates until a given sequence number, and
     * corrects that sn's update with the proper address and taken/not taken.
     * @param squashed_sn The sequence number to squash any younger updates up
     * until.
     * @param corr_target The correct branch target.
     * @param actually_taken The correct branch direction.
     * @param tid The thread id.
     */
    void squash(const InstSeqNum &squashed_sn,
                const TheISA::PCState &corr_target,
                bool actually_taken, ThreadID tid);

    /**
     * @param bp_history Pointer to the history object.  The predictor
     * will need to update any state and delete the object.
     */
    void BPSquash(void *bp_history);

    /**
     * Looks up a given PC in the BP to see if it is taken or not taken.
     * @param inst_PC The PC to look up.
     * @param bp_history Pointer that will be set to an object that
     * has the branch predictor state associated with the lookup.
     * @return Whether the branch is taken or not taken.
     */
    bool BPLookup(Addr instPC, void * &bp_history, int positionInFetchGroup, bool onDemand, bool &choicePred, bool onlyLP);
    void noBPLookup(Addr instPC, void * &bp_history, int positionInLine);

     /**
     * If a branch is not taken, because the BTB address is invalid or missing,
     * this function sets the appropriate counter in the global and local
     * predictors to not taken.
     * @param inst_PC The PC to look up the local predictor.
     * @param bp_history Pointer that will be set to an object that
     * has the branch predictor state associated with the lookup.
     */
    void BPBTBUpdate(Addr instPC, void * &bp_history);

    /**
     * Looks up a given PC in the BTB to see if a matching entry exists.
     * @param inst_PC The PC to look up.
     * @return Whether the BTB contains the given PC.
     */
    bool BTBValid(Addr instPC)
    { return BTB.valid(instPC, 0); }

    /**
     * Looks up a given PC in the BTB to get the predicted target.
     * @param inst_PC The PC to look up.
     * @return The address of the target of the branch.
     */
    TheISA::PCState BTBLookup(Addr instPC)
    { return BTB.lookup(instPC, 0); }

    /**
     * Updates the BP with taken/not taken information.
     * @param inst_PC The branch's PC that will be updated.
     * @param taken Whether the branch was taken or not taken.
     * @param bp_history Pointer to the branch predictor state that is
     * associated with the branch lookup that is being updated.
     * @param squashed Set to true when this function is called during a
     * squash operation.
     * @todo Make this update flexible enough to handle a global predictor.
     */
    void BPUpdate(Addr instPC, bool taken, void *bp_history, bool squashed);
    void BPHistUpdate(Addr instPC, bool taken, void *bp_history, bool squashed);
	void BPHistGarbageCollect(void *bp_history);

    /**
     * Updates the BTB with the target of a branch.
     * @param inst_PC The branch's PC that will be updated.
     * @param target_PC The branch's target that will be added to the BTB.
     */
    void BTBUpdate(Addr instPC, const TheISA::PCState &target)
    { BTB.update(instPC, target, 0); }

    void dump();

  private:
    struct PredictorHistory {
        /**
         * Makes a predictor history struct that contains any
         * information needed to update the predictor, BTB, and RAS.
         */
        PredictorHistory(const InstSeqNum &seq_num, Addr instPC,
                         bool pred_taken, void *bp_history,
                         ThreadID _tid)
            : seqNum(seq_num), pc(instPC), bpHistory(bp_history), RASTarget(0),
              RASIndex(0), tid(_tid), predTaken(pred_taken), usedRAS(0),
              wasCall(0), wasReturn(0), validBTB(0)
        {}

        bool operator==(const PredictorHistory &entry) const {
            return this->seqNum == entry.seqNum;
        }

        /** The sequence number for the predictor history entry. */
        InstSeqNum seqNum;

        /** The PC associated with the sequence number. */
        Addr pc;

        /** Pointer to the history object passed back from the branch
         * predictor.  It is used to update or restore state of the
         * branch predictor.
         */
        void *bpHistory;

        /** The RAS target (only valid if a return). */
        TheISA::PCState RASTarget;

        /** The RAS index of the instruction (only valid if a call). */
        unsigned RASIndex;

        /** The thread id. */
        ThreadID tid;

        /** Whether or not it was predicted taken. */
        bool predTaken;

        /** Whether or not the RAS was used. */
        bool usedRAS;

        /** Whether or not the instruction was a call. */
        bool wasCall;

        /** Whether or not the instruction was a return. */
        bool wasReturn;
        /** Whether or not the instruction had a valid BTB entry. */
        bool validBTB;

		/** Wheather or not it is a control **/
		bool wasControl;

		/** Wheather or not it is statically predicted **/
		bool wasStatic;

		/** Wheather or not it is static taken **/
		bool wasST;
    };

    typedef std::list<PredictorHistory> History;
    typedef typename History::iterator HistoryIt;

    /**
     * The per-thread predictor history. This is used to update the predictor
     * as instructions are committed, or restore it to the proper state after
     * a squash.
     */
    History predHist[Impl::MaxThreads];

    /** The local branch predictor. */
    LocalBP *localBP;

    /** The tournament branch predictor. */
    HybridBP *hybridbp;
    HybridBPskew *hybridSkewbp;

    /** The BTB. */
    DefaultBTB BTB;

    /** The per-thread return address stack. */
    ReturnAddrStack RAS[Impl::MaxThreads];

    /** Stat for number of BP lookups. */
    Stats::Scalar lookups;
    Stats::Scalar lookup_and_updates;
    /** Stat for number of conditional branches predicted. */
    Stats::Scalar condPredicted;
    /** Stat for number of conditional branches predicted incorrectly. */
    Stats::Scalar condIncorrect;
    /** Stat for number of BTB lookups. */
    Stats::Scalar BTBLookups;
    /** Stat for number of BTB hits. */
    Stats::Scalar BTBHits;
    /** Stat for number of times the BTB is correct. */
    Stats::Scalar BTBCorrect;
    /** Stat for number of times the RAS is used to get a target. */
    Stats::Scalar usedRAS;
    /** Stat for number of times the RAS is incorrect. */
    Stats::Scalar RASIncorrect;


	/** ODBP measurements */
    Stats::Scalar DY_treated_SN;
    Stats::Scalar ST_treated_SN;
    Stats::Formula DY_ST_treated_SN;
	//Stats::SparseHistogram bpIndxHist;

	/* enable / disable ODBP */
	bool onDemand;

	/* instruction cache fetch width */
	bool fetchWidth;

    /** Number of bits to shift the instruction over to get rid of the word
     *  offset.
     */
    unsigned instShiftAmt;

    /** Number of bits to shift the instruction over to get rid of the fetchwidth
     *  offset AND word offset.
     */
    unsigned totalInstShiftAmt;

};

#endif // __CPU_O3_BPRED_UNIT_HH__
