//
// Created by Henri on 25/03/2026.
//

#ifndef IDOL_CGLCUTS_H
#define IDOL_CGLCUTS_H

#include "idol/mixed-integer/optimizers/branch-and-bound/callbacks/BranchAndBoundCallbackFactory.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/callbacks/BranchAndBoundCallback.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"
#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/CutFamily.h"

#ifdef IDOL_USE_CGL
#include <CglKnapsackCover.hpp>
#include <CglFlowCover.hpp>
#include <CglZeroHalf.hpp>
#include <CglMixedIntegerRounding.hpp>
#include <CglOddHole.hpp>
#include <CglLandP.hpp>
#include <CglResidualCapacity.hpp>
#else
class OsiRowCut;
#endif

namespace idol {
    template<class> class CglCutCallback;
}

template<class NodeInfoT = idol::DefaultNodeInfo>
class idol::CglCutCallback : public BranchAndBoundCallbackFactory<NodeInfoT> {
public:
    class Strategy : public BranchAndBoundCallback<NodeInfoT> {

        class NodeCutContext {
            const unsigned int m_node_id;
            unsigned int m_pass_count = 0;
            unsigned int m_n_accepted_cuts = 0;
        public:
            NodeCutContext(unsigned int node_id) : m_node_id(node_id) {}

            [[nodiscard]] unsigned int node_id() const { return m_node_id; }
            [[nodiscard]] bool is_root_node() const { return (m_node_id == 0); }
            [[nodiscard]] unsigned int pass() const { return m_pass_count; }
            [[nodiscard]] unsigned int n_added_cuts() const { return m_n_accepted_cuts; }

            void increment_pass() { m_pass_count++; }
            void add_accepted_cut() { m_n_accepted_cuts++; }
        };

        class OsiIdolCglSolverInterface;
#ifdef IDOL_USE_CGL
        std::unique_ptr<OsiIdolCglSolverInterface> m_osi_solver;
#endif
        std::vector<std::unique_ptr<CutFamily>> m_cut_families;
        std::unique_ptr<NodeCutContext> m_cut_context;

        unsigned int m_total_n_added_cuts = 0;
        const unsigned int m_max_pass_at_aggressive_node = 20;
        const unsigned int m_max_pass_per_regular_node = 1;
        const unsigned int m_max_depth_for_cuts = 50;
        const double m_effectiveness_threshold = 1e-3;
        const unsigned int m_max_cut_at_aggressive_node = std::numeric_limits<unsigned int>::max();
        const unsigned int m_max_cut_per_regular_node = std::numeric_limits<unsigned int>::max();
        const unsigned int m_max_cuts = std::numeric_limits<unsigned int>::max();

        std::list<TempCtr> to_idol_cuts(OsiCuts& t_cuts);
        TempCtr to_idol_cut(OsiRowCut& t_cut);
        std::vector<std::pair<TempCtr, double>> sort_cuts_by_effectiveness(const std::list<TempCtr>& t_cuts);
    protected:
        NodeCutContext& get_cut_context();
        const NodeCutContext& get_cut_context() const { return const_cast<Strategy*>(this)->get_cut_context(); }
        void initialize() override;
        void log_after_termination() override;
        void operator()(CallbackEvent t_event) override;
    };

    Strategy* operator()() override;
    [[nodiscard]] CglCutCallback* clone() const override;
};

template <class NodeInfoT>
idol::CglCutCallback<NodeInfoT>::Strategy* idol::CglCutCallback<NodeInfoT>::operator()() {
    return new Strategy();
}

template <class NodeInfoT>
idol::CglCutCallback<NodeInfoT>* idol::CglCutCallback<NodeInfoT>::clone() const {
    return new CglCutCallback<NodeInfoT>(*this);
}

#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/OsiIdolCglSolverInterface.h"

template <class NodeInfoT>
void idol::CglCutCallback<NodeInfoT>::Strategy::initialize() {
#ifdef IDOL_USE_CGL

    BranchAndBoundCallback<NodeInfoT>::initialize();

    m_osi_solver = std::make_unique<OsiIdolCglSolverInterface>(*this);

    m_cut_families.emplace_back(new CutFamily(new CglKnapsackCover(), "Cover"));
    m_cut_families.emplace_back(new CutFamily(new CglFlowCover(), "Flow Cover"));
    m_cut_families.emplace_back(new CutFamily(new CglZeroHalf(), "Zero Half"));
    m_cut_families.emplace_back(new CutFamily(new CglMixedIntegerRounding(), "MIR"));
    m_cut_families.emplace_back(new CutFamily(new CglResidualCapacity(), "Residual Capacity"));
    //m_cut_families.emplace_back(new CutFamily(new CglLandP(), "Lift-and-Project")); // Needs getObjValue
    //m_cut_families.emplace_back(new CutFamily(new CglOddHole(), "Odd Hole")); // Needs reduced costs

#else
    throw Exception("idol was not linked with Cgl.");
#endif
}

template <class NodeInfoT>
void idol::CglCutCallback<NodeInfoT>::Strategy::log_after_termination() {

    BranchAndBoundCallback<NodeInfoT>::log_after_termination();

    std::cout << "Cgl Cutting Planes:" << std::endl;

    for (const auto& cut_family : m_cut_families) {
        std::cout << '\t' << cut_family->name() << ": " << cut_family->n_accepted() << "\n";
    }

}

template <class NodeInfoT>
void idol::CglCutCallback<NodeInfoT>::Strategy::operator()(CallbackEvent t_event) {
#ifdef IDOL_USE_CGL
    if (t_event != InvalidSolution) {
        return;
    }
    
    /////////////////////////////////

    const auto EPSILON_ = 1e-4;

    const auto flipRow=  [](int rowLen, double* coef, char& sen,
              double& rhs)  {
        for(int i = 0; i < rowLen; ++i) coef[i] = -coef[i];
        sen = (sen == 'G') ?  'L' : 'G';
        rhs = -rhs;
    };

    const auto determineOneRowType = [&](const OsiSolverInterface& si,
				  int rowLen, int* ind,
				  double* coef, char sense,
				  double rhs)
        {
          if (rowLen == 0)
            return CGLFLOW_ROW_UNDEFINED;
          if (sense == 'R')
            return CGLFLOW_ROW_UNINTERSTED; // Could be fixed

          CglFlowRowType rowType = CGLFLOW_ROW_UNDEFINED;
          // Get integer types
          const char * columnType = si.getColType ();

          int  numPosBin = 0;      // num of positive binary variables
          int  numNegBin = 0;      // num of negative binary variables
          int  numBin    = 0;      // num of binary variables
          int  numPosCol = 0;      // num of positive variables
          int  numNegCol = 0;      // num of negative variables
          int  i;
          bool flipped = false;

          // Range row will only consider as 'L'
          if (sense == 'G') {        // Transform to " <= "
            flipRow(rowLen, coef, sense, rhs);
            flipped = true;
          }

          // Summarize the variable types of the given row.
          for ( i = 0; i < rowLen; ++i ) {
            if ( coef[i] < -EPSILON_ ) {
              ++numNegCol;
              if( columnType[ind[i]]==1 )
	        ++numNegBin;
            }
            else {
              ++numPosCol;
              if( columnType[ind[i]]==1 )
	        ++numPosBin;
            }
          }
          numBin = numNegBin + numPosBin;

          //------------------------------------------------------------------------
          // Classify row type based on the types of variables.

          // All variables are binary. NOT interested in this type of row right now
          if (numBin == rowLen)
            rowType = CGLFLOW_ROW_UNINTERSTED;

          // All variables are NOT binary
          if (rowType == CGLFLOW_ROW_UNDEFINED && numBin == 0) {
            if (sense == 'L')
              rowType = CGLFLOW_ROW_NOBINUB;
            else
              rowType = CGLFLOW_ROW_NOBINEQ;
          }

          // There are binary and other types of variables
          if (rowType == CGLFLOW_ROW_UNDEFINED) {
            if ((rhs < -EPSILON_) || (rhs > EPSILON_) || (numBin != 1)) {
              if (sense == 'L')
	        rowType = CGLFLOW_ROW_MIXUB;
              else
	        rowType = CGLFLOW_ROW_MIXEQ;
            }
            else {                               // EXACTLY one binary
              if (rowLen == 2) {               // One binary and one other type
	        if (sense == 'L') {
	          if (numNegCol == 1 && numNegBin == 1)
	            rowType = CGLFLOW_ROW_VARUB;
	          if (numPosCol == 1 && numPosBin == 1)
	            rowType = CGLFLOW_ROW_VARLB;
	        }
	        else
	          rowType = CGLFLOW_ROW_VAREQ;
              }
              else {               // One binary and 2 or more other types
	        if (numNegCol==1 && numNegBin==1) {// Binary has neg coef and
	          if (sense == 'L')  // other are positive
	            rowType = CGLFLOW_ROW_SUMVARUB;
	          else
	            rowType = CGLFLOW_ROW_SUMVAREQ;
	        }
              }
            }
          }

          // Still undefined
          if (rowType == CGLFLOW_ROW_UNDEFINED) {
            if (sense == 'L')
              rowType = CGLFLOW_ROW_MIXUB;
            else
              rowType = CGLFLOW_ROW_MIXEQ;
          }
          if (flipped == true) {
            flipRow(rowLen, coef, sense, rhs);
          }

          return rowType;
        };

        CoinPackedMatrix matrixByRow(*m_osi_solver->getMatrixByRow());

      int numRows = m_osi_solver->getNumRows();
      int numCols = m_osi_solver->getNumCols();

      const char* sense        = m_osi_solver->getRowSense();
      const double* RHS        = m_osi_solver->getRightHandSide();

      const double* coefByRow  = matrixByRow.getElements();
      const int* colInds       = matrixByRow.getIndices();
      const CoinBigIndex* rowStarts     = matrixByRow.getVectorStarts();
      const int* rowLengths    = matrixByRow.getVectorLengths();
      int iRow      = -1;
      int iCol      = -1;

      auto numCols_ = numCols;     // Record col and row numbers for copy constructor
      auto numRows_ = numRows;

      CglFlowRowType* rowTypes_ = 0;
      if (rowTypes_ != 0) {
        delete [] rowTypes_; rowTypes_ = 0;
      }
      rowTypes_ = new CglFlowRowType [numRows];// Destructor will free memory
      // Get integer types
      const char * columnType = m_osi_solver->getColType (true);

      // Summarize the row type infomation.
      int numUNDEFINED   = 0;
      int numVARUB       = 0;
      int numVARLB       = 0;
      int numVAREQ       = 0;
      int numMIXUB       = 0;
      int numMIXEQ       = 0;
      int numNOBINUB     = 0;
      int numNOBINEQ     = 0;
      int numSUMVARUB    = 0;
      int numSUMVAREQ    = 0;
      int numUNINTERSTED = 0;

      int* ind     = new int [numCols];
      double* coef = new double [numCols];
      for (iRow = 0; iRow < numRows; ++iRow) {
        int rowLen   = rowLengths[iRow];
        char sen     = sense[iRow];
        double rhs   = RHS[iRow];

        CoinDisjointCopyN(colInds + rowStarts[iRow], rowLen, ind);
        CoinDisjointCopyN(coefByRow + rowStarts[iRow], rowLen, coef);

        CglFlowRowType rowType = determineOneRowType(*m_osi_solver, rowLen, ind, coef,
						     sen, rhs);

        rowTypes_[iRow] = rowType;

        switch(rowType) {
        case  CGLFLOW_ROW_UNDEFINED:
          ++numUNDEFINED;
          break;
        case  CGLFLOW_ROW_VARUB:
          ++numVARUB;
          break;
        case  CGLFLOW_ROW_VARLB:
          ++numVARLB;
          break;
        case  CGLFLOW_ROW_VAREQ:
          ++numVAREQ;
          break;
        case  CGLFLOW_ROW_MIXUB:
          ++numMIXUB;
          break;
        case  CGLFLOW_ROW_MIXEQ:
          ++numMIXEQ;
          break;
        case  CGLFLOW_ROW_NOBINUB:
          ++numNOBINUB;
          break;
        case  CGLFLOW_ROW_NOBINEQ:
          ++numNOBINEQ;
          break;
        case  CGLFLOW_ROW_SUMVARUB:
          ++numSUMVARUB;
          break;
        case  CGLFLOW_ROW_SUMVAREQ:
          ++numSUMVAREQ;
          break;
        case  CGLFLOW_ROW_UNINTERSTED:
          ++numUNINTERSTED;
          break;
        default:
          throw CoinError("Unknown row type", "flowPreprocess",
		          "CglFlowCover");
        }

      }
      delete [] ind;  ind  = NULL;
      delete [] coef; coef = NULL;

      //---------------------------------------------------------------------------
      // Setup  vubs_ and vlbs_
      CglFlowVUB* vubs_ = 0;
      CglFlowVLB* vlbs_ = 0;
      if (vubs_ != 0) { delete [] vubs_; vubs_ = 0; }
      vubs_ = new CglFlowVUB [numCols];      // Destructor will free memory
      if (vlbs_ != 0) { delete [] vlbs_; vlbs_ = 0; }
      vlbs_ = new CglFlowVLB [numCols];      // Destructor will free memory

      for (iCol = 0; iCol < numCols; ++iCol) {   // Initilized in constructor
        vubs_[iCol].setVar(-1);     // but, need redo since may call
        vlbs_[iCol].setVar(-1);     // preprocess(...) more than once
      }

      for (iRow = 0; iRow < numRows; ++iRow) {

        CglFlowRowType rowType2 = rowTypes_[iRow];

        if ( (rowType2 == CGLFLOW_ROW_VARUB) ||
	     (rowType2 == CGLFLOW_ROW_VARLB) ||
	     (rowType2 == CGLFLOW_ROW_VAREQ) )  {

          CoinBigIndex startPos = rowStarts[iRow];
          int index0   = colInds[startPos];
          int index1   = colInds[startPos + 1];
          double coef0 = coefByRow[startPos];
          double coef1 = coefByRow[startPos + 1];

          int    xInd,  yInd;   // x is binary
          double xCoef, yCoef;

          if ( columnType[index0]==1 ) {
	    xInd  = index0;   yInd  = index1;
	    xCoef = coef0;    yCoef = coef1;
          }
          else {
	    xInd  = index1;   yInd  = index0;
	    xCoef = coef1;    yCoef = coef0;
          }

          switch (rowType2) {
          case CGLFLOW_ROW_VARUB:       // Inequality: y <= ? * x
	    vubs_[yInd].setVar(xInd);
	    vubs_[yInd].setVal(-xCoef / yCoef);
	    break;
          case CGLFLOW_ROW_VARLB:       // Inequality: y >= ? * x
	    vlbs_[yInd].setVar(xInd);
	    vlbs_[yInd].setVal(-xCoef / yCoef);
	    break;
          case CGLFLOW_ROW_VAREQ:       // Inequality: y >= AND <= ? * x
	    vubs_[yInd].setVar(xInd);
	    vubs_[yInd].setVal(-xCoef / yCoef);
	    vlbs_[yInd].setVar(xInd);
	    vlbs_[yInd].setVal(-xCoef / yCoef);
	    break;
          default:
	    throw CoinError("Unknown row type: impossible",
			    "flowPreprocess", "CglFlowCover");
          }
        }
      }

    /////////////////////////////////

    if (m_total_n_added_cuts > m_max_cuts) {
        return;
    }

    if (this->node().level() > m_max_depth_for_cuts) {
        return;
    }

    const double relative_gap = idol::relative_gap(this->best_bound(), this->best_obj()) * 100;

    if (relative_gap < 1) {
        return;
    }

    auto& cut_context = get_cut_context();
    const bool is_root_node = cut_context.is_root_node();

    bool be_aggressive = false;
    if (is_root_node || relative_gap > 50) {
        be_aggressive = true;
    }

    unsigned int max_n_added_cut_at_this_node = m_max_cut_per_regular_node;
    unsigned int max_pass_at_this_node = m_max_pass_per_regular_node;
    if (be_aggressive) {
        max_n_added_cut_at_this_node = m_max_cut_at_aggressive_node;
        max_pass_at_this_node = m_max_pass_at_aggressive_node;
    }

    if (cut_context.pass() > max_pass_at_this_node) {
        return;
    }

    cut_context.increment_pass();

    // Count number of fractional over integer ?
    if (!be_aggressive) {
        std::sort(m_cut_families.begin(), m_cut_families.end(), [](const auto& t_a, const auto& t_b) {
            return t_a->score() > t_b->score();
        });
    }

    m_osi_solver->update_current_solution();

    auto& registry = this->side_effect_registry();
    for (auto& cut_family : m_cut_families) {

        if (!be_aggressive && cut_family->score() <= 1e-2) {
            continue;
        }

        auto osi_cuts = cut_family->generate(*m_osi_solver, be_aggressive ? 100 : 0);
        auto idol_cuts = to_idol_cuts(osi_cuts);
        auto sorted_cuts = sort_cuts_by_effectiveness(idol_cuts);

        for (auto& [cut, effectiveness] : sorted_cuts) {

            if (m_total_n_added_cuts > m_max_cuts) {
                return;
            }

            if (cut_context.n_added_cuts() > max_n_added_cut_at_this_node) {
                return;
            }

            cut_family->add_effectiveness_statistics(effectiveness);

            if (effectiveness <= m_effectiveness_threshold) {
                continue;
            }

            const unsigned int old_n_added_user_cuts = registry.n_added_user_cuts;
            this->add_user_cut(cut);
            const bool cut_was_accepted_by_idol = registry.n_added_user_cuts > old_n_added_user_cuts;

            if (cut_was_accepted_by_idol) {
                m_total_n_added_cuts++;
                cut_context.add_accepted_cut();
                cut_family->add_accepted_cut();
            }
        }
    }
#endif
}

template <class NodeInfoT>
std::list<idol::TempCtr> idol::CglCutCallback<NodeInfoT>::Strategy::to_idol_cuts(OsiCuts& t_cuts) {
#ifdef IDOL_USE_CGL
    std::list<TempCtr> result;

    for (unsigned int k = 0, n = t_cuts.sizeRowCuts(); k < n; k++) {
        auto& cut = *t_cuts.rowCutPtr(k);

        if (!cut.consistent()) {
            continue;
        }

        if (cut.infeasible(*m_osi_solver)) {
            continue;
        }

        result.emplace_back(to_idol_cut(cut));
    }

    return result;
#else
    throw Exception("idol was not linked with Cgl.");
#endif
}

template <class NodeInfoT>
idol::TempCtr idol::CglCutCallback<NodeInfoT>::Strategy::to_idol_cut(OsiRowCut& t_cut) {
#ifdef IDOL_USE_CGL
    const auto& model = this->original_model();

    TempCtr cut;

    const auto osi_sense = t_cut.sense();
    if (osi_sense == 'G') {
        cut.set_type(GreaterOrEqual);
        cut.set_rhs(t_cut.lb());
    }
    else if (osi_sense == 'L') {
        cut.set_type(LessOrEqual);
        cut.set_rhs(t_cut.ub());
    }
    else if (osi_sense == 'R') {
        if (const double lb = t_cut.lb(); !is_neg_inf(lb)) {
            cut.set_type(GreaterOrEqual);
            cut.set_rhs(t_cut.lb());
        }
        else if (const double ub = t_cut.ub(); !is_pos_inf(ub)) {
            cut.set_type(LessOrEqual);
            cut.set_rhs(t_cut.ub());
        }
        else {
            throw Exception("Cgl returned a cut of type R, which is not handled in idol.");
        }
    }
    else {
        throw Exception("Could not handle cut type from Cgl.");
    }

    const auto& osi_row = t_cut.row();
    const auto* indices = osi_row.getIndices();
    const auto* values = osi_row.getElements();

    for (unsigned int k = 0, n = osi_row.getNumElements(); k < n; k++) {
        const auto& var = model.get_var_by_index(indices[k]);
        cut.lhs() += values[k] * var;
    }

    return cut;
#else
    throw Exception("idol was not linked with Cgl.");
#endif
}

template <class NodeInfoT>
std::vector<std::pair<idol::TempCtr, double>> idol::CglCutCallback<NodeInfoT>::Strategy::sort_cuts_by_effectiveness(
    const std::list<TempCtr>& t_cuts) {
    std::vector<std::pair<TempCtr, double>> result;
    const auto primal_solution = this->node().info().primal_solution();

    for (auto& cut : t_cuts) {
        double activity = 0.;
        double norm = 0.;

        for (const auto& [var, coefficient] : cut.lhs()) {
            norm += coefficient * coefficient;
            activity += coefficient * primal_solution.get(var);
        }
        norm = std::sqrt(norm);

        double effectiveness = (activity - cut.rhs()) / norm;
        if (cut.type() == GreaterOrEqual) {
            effectiveness *= -1.;
        }

        result.emplace_back(std::move(cut), effectiveness);
    }

    std::sort(result.begin(), result.end(), [](const auto& t_a, const auto& t_b) {
        return t_a.second > t_b.second;
    });

    return result;
}

template <class NodeInfoT>
idol::CglCutCallback<NodeInfoT>::Strategy::NodeCutContext& idol::CglCutCallback<
    NodeInfoT>::Strategy::get_cut_context() {
    const auto current_node_id = this->node().id();

    if (!m_cut_context || m_cut_context->node_id() != current_node_id) {
        m_cut_context.reset(new NodeCutContext(current_node_id));
    }

    return *m_cut_context;
}

#endif //IDOL_CGLCUTS_H
