//
// Created by Henri on 30/04/2026.
//
#include "idol/robust/optimizers/wrappers/yasol/QpExternSolverWithSnapshot.h"

#include <cassert>

idol::impl::QpExternSolverWithSnapshot::QpExternSolverWithSnapshot(QpExtSolSolverType t, QpExtSolSolverMode m)
    : QpExternSolver(t, m) {}

int idol::impl::QpExternSolverWithSnapshot::computeStatus(const std::vector<data::IndexedElement>& lhs, const data::QpRhs& rhs) {
	double multiplier=1.0;
	int status = 0;
	if (rhs.getRatioSign() == data::QpRhs::RatioSign::equal) return 0;
	if (rhs.getRatioSign() == data::QpRhs::RatioSign::smallerThanOrEqual)
		multiplier = -1.0;

	double cntPos=0.0;
	double cntNeg=0.0;
	bool all01 = true;
	for (int i = 0; i < lhs.size();i++) {
		double cf = lhs[i].value.asDouble();
		if (fabs(cf) > -1e-12 && fabs(cf) < 1e-12)
			;
		else if (fabs(cf) > 1.0-1e-12 && fabs(cf) < 1.0+1e-12)
			;
		else all01 = false;
		if (multiplier * cf > 0.0) cntPos=cntPos+1.0;
		if (multiplier * cf < 0.0) cntNeg=cntNeg+1.0;
	}
	double lrhs = multiplier*rhs.getValue().asDouble();
	if (all01 && fabs(floor(lrhs + 0.5)-lrhs) < 1e-10 && fabs(1.0-cntNeg - lrhs) < 1e-12  )
		status |= 1;
	//if ((double)lhs.size() - cntPos < 0.5) status |= 2;
	return status;
}

void idol::impl::QpExternSolverWithSnapshot::setStatus(int i, int j) {
	if (i >= indicators.size()) indicators.resize(i+1);
	indicators[i] = j;
}

int idol::impl::QpExternSolverWithSnapshot::getStatus(int i) {
	return indicators[i];
}

void idol::impl::QpExternSolverWithSnapshot::setLazyStatus(int i, bool s) {
	if (i < 0) {
		lazyRows.clear();
		for (int j = 0; j < lazyRowIndicator.size();j++) {
			lazyRowIndicator[j] = false;
		}
		return;
	}
	if(i>=lazyRowIndicator.size()) {
#ifdef SHOW_EXTERN_SOLVER_WARNINGS
		std::cerr << "Error: lazyRowIndicator.size() = " << lazyRowIndicator.size() << " but i=" << i << std::endl;
#endif
		return;
	}
	if (lazyRowIndicator[i] == false && s == true)
		lazyRows.push_back(i);
	lazyRowIndicator[i] = s;
}

bool idol::impl::QpExternSolverWithSnapshot::getLazyStatus(int i) {
	return lazyRowIndicator[i];
}

bool idol::impl::QpExternSolverWithSnapshot::getLazyRows(std::vector<int>& lR, std::vector<data::QpNum>& solution,
	double eps) {
	if (solution.size() == 0) return false;
#ifdef ggjgj
    lR.clear();
    for (int i = 0; i < RHSs.size();i++) {
        int ri = i;
        if (lazyRowIndicator[ri] == false) continue;
        double lhs=0.0;
        for (int j = 0; j < LHSs[ri].size();j++) {
            lhs = lhs + LHSs[ri][j].value.asDouble() * solution[LHSs[ri][j].index].asDouble();
        }
        if (RHSs[ri].getRatioSign() == data::QpRhs::RatioSign::greaterThanOrEqual) {
            if (lhs <= RHSs[ri].getValue().asDouble()+eps )
                lR.push_back(ri);
        } else if (RHSs[ri].getRatioSign() == data::QpRhs::RatioSign::smallerThanOrEqual) {
            if (lhs >= RHSs[ri].getValue().asDouble()-eps )
                lR.push_back(ri);
        } else {
            if (fabs(lhs-RHSs[ri].getValue().asDouble()) > 0/*eps*/) lR.push_back(ri);
        }
    }
    if (0)for (int i = 0; i < lazyRows.size();i++) {
        int ri = lazyRows[i];
        if (lazyRowIndicator[ri] == false) continue;
        double lhs=0.0;
        for (int j = 0; j < LHSs[ri].size();j++) {
            lhs = lhs + LHSs[ri][j].value.asDouble() * solution[LHSs[ri][j].index].asDouble();
        }
        if (RHSs[ri].getRatioSign() == data::QpRhs::RatioSign::greaterThanOrEqual) {
            if (lhs < RHSs[ri].getValue().asDouble()+eps )
                lR.push_back(ri);
        } else if (RHSs[ri].getRatioSign() == data::QpRhs::RatioSign::smallerThanOrEqual) {
            if (lhs > RHSs[ri].getValue().asDouble()-eps )
                lR.push_back(ri);
        } else {
            if (fabs(lhs-RHSs[ri].getValue().asDouble()) < eps) lR.push_back(ri);
        }
    }
    if(0)for (int i = 0; i < lR.size();i++)
        lazyRowIndicator[lR[i]] = false;
    if(0)for (int i = 0; i < lazyRows.size();i++) {
        while(lazyRows.size() > 0 && lazyRowIndicator[ lazyRows[lazyRows.size()-1] ] == false)
            lazyRows.pop_back();
        if (lazyRows.size() == 0) break;
        if (i >= lazyRows.size()) break;
        if (lazyRowIndicator[ lazyRows[i] ] == false) {
            assert(lazyRowIndicator[ lazyRows[lazyRows.size()-1] ] == true);
            lazyRows[i] = lazyRows[lazyRows.size()-1];
            lazyRows.pop_back();
        }
    }
    if (lR.size() > 0) return true;
    else return false;
#endif
    lR.clear();
    for (int i = 0; i < lazyRows.size();i++) {
        int ri = lazyRows[i];
	if (ri < 0 || ri >= RHSs.size()) continue;
        if (lazyRowIndicator[ri] == false) continue;
        double lhs=0.0;
        //eps = -1e-10+0.0;//-1e-7;//0.001 + fabs(0.001 * RHSs[ri].getValue().asDouble());
        eps = -0.001 - fabs(0.001 * RHSs[ri].getValue().asDouble());
	bool bigX=false;
        for (int j = 0; j < LHSs[ri].size();j++) {
	  assert(ri < LHSs.size());
	  assert(j < LHSs[ri].size());
	  //assert(LHSs[ri][j].index < solution.size());
	  if (LHSs[ri][j].index >= solution.size()) {
	    bigX = true;
	    break;
	  }
	  lhs = lhs + LHSs[ri][j].value.asDouble() * solution[LHSs[ri][j].index].asDouble();
        }
	if (bigX) {
	  //lazyRowIndicator[ri] = false;
	  //std::cerr << "X";
	  continue;
	}
        if (RHSs[ri].getRatioSign() == data::QpRhs::RatioSign::greaterThanOrEqual) {
            if (lhs < RHSs[ri].getValue().asDouble()+eps )
                lR.push_back(ri);
        } else if (RHSs[ri].getRatioSign() == data::QpRhs::RatioSign::smallerThanOrEqual) {
            if (lhs > RHSs[ri].getValue().asDouble()-eps )
                lR.push_back(ri);
        } else {
            if (fabs(lhs-RHSs[ri].getValue().asDouble()) > 1e-10+0.0/*eps*/) lR.push_back(ri);
        }
    }
    for (int i = 0; i < lR.size();i++)
        lazyRowIndicator[lR[i]] = false;
    for (int i = 0; i < lazyRows.size();i++) {
        while(lazyRows.size() > 0 && lazyRowIndicator[ lazyRows[lazyRows.size()-1] ] == false)
            lazyRows.pop_back();
        if (lazyRows.size() == 0) break;
        if (i >= lazyRows.size()) break;
        if (lazyRowIndicator[ lazyRows[i] ] == false) {
            assert(lazyRowIndicator[ lazyRows[lazyRows.size()-1] ] == true);
            lazyRows[i] = lazyRows[lazyRows.size()-1];
            lazyRows.pop_back();
        }
    }
    //std::cerr << "LAZYROWS:" << lR.size() << std::endl;
    if (lR.size() > 0) return true;
    else return false;
}

void idol::impl::QpExternSolverWithSnapshot::clearLP_snapshot() {
    clearLP_snapshot(0);
    obj_lhs.clear();
    for (int i = 0; i < COLs.size();i++) COLs[i].clear();
}

void idol::impl::QpExternSolverWithSnapshot::clearLP_snapshot(int from) {
    for (int i = from; i < LHSs.size();i++) LHSs[i].clear();
    while (RHSs.size() > from) {
        lazyRowIndicator.pop_back();
        RHSs.pop_back();
        LHSs.pop_back();
    }
    lazyRows.clear();
    for (int i = 0; i < lazyRowIndicator.size();i++)
        if (lazyRowIndicator[i] == true)
            lazyRows.push_back(i);
}

void idol::impl::QpExternSolverWithSnapshot::clearLP_snapshot(int from, int to) {
    int second_start = to+1;
    if (second_start >= RHSs.size()) clearLP_snapshot(from);
    else {
        for (int i = from; i <= to && i < LHSs.size();i++) LHSs[i].clear();
        int f = from;
        int t = to;
        int new_last = RHSs.size() - t-f+1;
        for (;t < RHSs.size();f++,t++) {
            LHSs[f] = LHSs[t];
            RHSs[f] = RHSs[t];
            lazyRowIndicator[f] = lazyRowIndicator[t];
        }
        while (RHSs.size() > new_last) {
            lazyRowIndicator.pop_back();
            RHSs.pop_back();
            LHSs.pop_back();
        }
        lazyRows.clear();
        for (int i = 0; i < lazyRowIndicator.size();i++)
            if (lazyRowIndicator[i] == true)
                lazyRows.push_back(i);
    }
}

void idol::impl::QpExternSolverWithSnapshot::saveSnapshot() {
    for (int i = 0; i < LHSsSaved.size();i++) LHSsSaved[i].clear();
    while (RHSsSaved.size() > 0) {
        RHSsSaved.pop_back();
        LHSsSaved.pop_back();
    }
    obj_lhs_saved.clear();
    for (int i = 0; i < LHSs.size();i++) LHSsSaved.push_back(LHSs[i]);
    for (int i = 0; i < RHSs.size();i++) RHSsSaved.push_back(RHSs[i]);
    for (int i = 0; i < obj_lhs.size();i++) obj_lhs_saved.push_back(obj_lhs[i]);
    obj_lhs_dense_saved.resize(obj_lhs_dense.size());
    for (int i = 0; i < obj_lhs_dense.size();i++) obj_lhs_dense_saved[i] = obj_lhs_dense[i];
}

void idol::impl::QpExternSolverWithSnapshot::retrieveSnapshot() {
    for (int i = 0; i < LHSs.size();i++) LHSs[i].clear();
    while (RHSs.size() > 0) {
        RHSs.pop_back();
        LHSs.pop_back();
    }
    obj_lhs.clear();
    for (int i = 0; i < LHSsSaved.size();i++) LHSs.push_back(LHSsSaved[i]);
    for (int i = 0; i < RHSsSaved.size();i++) RHSs.push_back(RHSsSaved[i]);
    for (int i = 0; i < obj_lhs_saved.size();i++) obj_lhs.push_back(obj_lhs_saved[i]);
    obj_lhs_dense.resize(obj_lhs_dense_saved.size());
    for (int i = 0; i < obj_lhs_dense_saved.size();i++) obj_lhs_dense[i] = obj_lhs_dense_saved[i];
    lazyRows.clear();
    lazyRowIndicator.clear();
    for (int i = 0; i < RHSsSaved.size();i++) lazyRows.push_back(true);
    for (int i = 0; i < lazyRowIndicator.size();i++)
        if (lazyRowIndicator[i] == true)
            lazyRows.push_back(i);
}

void idol::impl::QpExternSolverWithSnapshot::addLProw_snapshot(const std::vector<data::IndexedElement>& lhs,
    const data::QpRhs& rhs) {
    // rows are normed: = or <= and: if = then leading coefficient > 0
    double sign_factor=1.0;
    data::QpRhs new_rhs = rhs;
    int status=0;
    if (new_rhs.getRatioSign() == data::QpRhs::RatioSign::greaterThanOrEqual) {
        sign_factor = -1.0;
        new_rhs.setRatioSign(data::QpRhs::RatioSign::smallerThanOrEqual);
        new_rhs.setValue(new_rhs.getValue().asDouble() * sign_factor);
    } else if (new_rhs.getRatioSign() == data::QpRhs::RatioSign::equal && lhs.size() > 0 && lhs[0].value.asDouble() < 0) {
        sign_factor = -1.0;
        new_rhs.setValue(new_rhs.getValue().asDouble() * sign_factor);
    }
    std::vector<data::IndexedElement> new_lhs;
    for (int i = 0; i < lhs.size();i++) {
        data::IndexedElement elem;
        elem.value = sign_factor * lhs[i].value.asDouble();
        elem.index = lhs[i].index;
        if (elem.value.asDouble() < 1e-12 && elem.value.asDouble() > -1e-12) continue;
        new_lhs.push_back(elem);
    }
    LHSs.push_back(new_lhs);
    RHSs.push_back(new_rhs);
    lazyRowIndicator.push_back(true);
    lazyRows.push_back(RHSs.size()-1);
    setStatus(RHSs.size()-1, computeStatus(new_lhs, new_rhs) );
}

void idol::impl::QpExternSolverWithSnapshot::addLPobj_snapshot(const std::vector<data::IndexedElement>& o_lhs,
    const data::QpRhs& o_rhs) {
    obj_lhs.clear();
    for (int i = 0; i < o_lhs.size();i++) obj_lhs.push_back(o_lhs[i]);
    obj_rhs = o_rhs;
    int n = getVariableCount();
    data::IndexedElement ie(0,0);
    obj_lhs_dense.resize(n);
    for (int i = 0; i < n;i++) obj_lhs_dense[i] = ie;
    for (int i = 0; i < obj_lhs.size();i++) obj_lhs_dense[obj_lhs[i].index] = obj_lhs[i];
}

void idol::impl::QpExternSolverWithSnapshot::initInternalLP_snapshot(const data::Qlp& qlp) {
    // builds a row and a column representation in the extern solver itself
	std::vector<const data::QpVar *> varVec = qlp.getVariableVectorConst();
	int cntVars = qlp.getVariableCount();
	int numConstraints = qlp.getConstraintCount();
	std::vector<const data::QpRhs *> rhsVec = qlp.getRhsVecConst();
	std::vector<const data::Constraint *> conVec = qlp.getConstraintVecConst();

	clearLP_snapshot();
	assert(lazyRowIndicator.size()==0);
	assert(lazyRows.size()==0);
	for (int i = 0; i < numConstraints;i++) {
	  data::QpRhs org_rhs = *rhsVec[i];
	  std::vector<data::IndexedElement> org_lhs = conVec[i]->getElements();

	  // split in leq-manner
	  data::QpNum multiplier = 1.0;
	  if (org_rhs.getRatioSign() == data::QpRhs::RatioSign::greaterThanOrEqual)
	    multiplier = -1.0;
	  std::vector<data::IndexedElement> negSummands;
	  std::vector<data::IndexedElement> posSummands;
	  std::vector<data::IndexedElement> indifferentSummands;
	  for (int j=0; j < org_lhs.size();j++) {
	    data::QpNum coef = org_lhs[j].value * multiplier;
	    data::QpNum lb = varVec[org_lhs[j].index]->getLowerBound();
	    data::QpNum ub = varVec[org_lhs[j].index]->getUpperBound();
	    if (coef >= 0.0 && lb >= 0.0)
	      posSummands.push_back(org_lhs[j]);
	    else if (coef <= 0.0 && ub <= 0.0)
	      posSummands.push_back(org_lhs[j]);
	    else if (coef <= 0.0 && lb >= 0.0)
	      negSummands.push_back(org_lhs[j]);
	    else if (coef >= 0.0 && ub <= 0.0)
	      negSummands.push_back(org_lhs[j]);
	    else indifferentSummands.push_back(org_lhs[j]);
	  }
	  data::QpNum sumPosCoefs = 0.0;
	  data::QpNum sumNegCoefs = 0.0;
	  for (int j=0;j<negSummands.size();j++)
	    sumNegCoefs = sumNegCoefs + fabs(negSummands[j].value.asDouble());
	  for (int j=0;j<posSummands.size();j++)
	    sumPosCoefs = sumPosCoefs + fabs(posSummands[j].value.asDouble());
	  if (indifferentSummands.size() == 0) {
	    if (negSummands.size() == 1 && posSummands.size() > 1 && varVec[negSummands[0].index]->getNumberSystem()==data::QpVar::binaries && org_rhs.getValue() < 1e-8 && org_rhs.getValue() > -1e-8) { // disaggregate
	      data::QpNum gegSummand0 = (negSummands[0].value >= 0 ? negSummands[0].value : data::QpNum(-1.0)*negSummands[0].value);
	      if ( gegSummand0 >= sumPosCoefs-1e-8 ) {
		std::cerr << "disaggregate!" << std::endl;
		for (int j=0;j<posSummands.size();j++) {
		  std::vector<data::IndexedElement> dummy_lhs(1);
		  dummy_lhs[0] = posSummands[j];
		  data::QpRhs dummy_rhs;
		  dummy_rhs.setRatioSign(data::QpRhs::RatioSign::smallerThanOrEqual);
		  dummy_rhs.setValue(0.0);
		  LHSs.push_back(dummy_lhs);
		  RHSs.push_back(dummy_rhs);
		  lazyRowIndicator.push_back(true);
		  lazyRows.push_back(RHSs.size()-1);
		  setStatus(RHSs.size()-1, computeStatus(dummy_lhs, dummy_rhs) );
		}
	      }
	    }
	  }

	  LHSs.push_back(org_lhs);
	  RHSs.push_back(org_rhs);
	  lazyRowIndicator.push_back(true);
	  lazyRows.push_back(RHSs.size()-1);
	  setStatus(RHSs.size()-1, computeStatus(org_lhs, org_rhs) );
	}

	const std::vector<data::QpNum>& tmpObjVec = qlp.getObjectiveFunctionValues();

	obj_lhs.clear();
	std::vector<data::IndexedElement> dummy;
	for (unsigned int i = 0; i < tmpObjVec.size(); i++) {
		COLs.push_back(dummy);
		if (!tmpObjVec[i].isZero())
			obj_lhs.push_back(data::IndexedElement(i, tmpObjVec[i]));
	}
	int n = getVariableCount();
	data::IndexedElement ie(0,0);
	obj_lhs_dense.resize(n);
	for (int i = 0; i < n;i++) obj_lhs_dense[i] = ie;
	for (int i = 0; i < obj_lhs.size();i++) obj_lhs_dense[obj_lhs[i].index] = obj_lhs[i];

	for (int i = 0; i < RHSs.size();i++) {
		for (int j = 0; j < LHSs[i].size();j++) {
			data::IndexedElement elem;
			elem.value = LHSs[i][j].value.asDouble();
			elem.index = i;//LHSs[i][j].index;
			COLs[LHSs[i][j].index].push_back(elem/*LHSs[i][j]*/);
			//COLs[LHSs[i][j].index][j].index = i;
		}
	}
}

void idol::impl::QpExternSolverWithSnapshot::reinitLPcols_snapshot() {
	// rebuilds the column representation in the extern solver itself
	// rows are made "<="
	// lhs of rows in col-representation are normed with the help of 1.0 / | LHS[i][0] |
	// obj are not normed
	// rhs is not normed, belongs to row representation
	int n = getVariableCount();
	for (int i = 0; i < COLs.size();i++) COLs[i].clear();
	std::vector<data::IndexedElement> dummy;
	while(COLs.size() < n)
		COLs.push_back(dummy);
	for (int i = 0; i < RHSs.size();i++) {
		data::QpRhs::RatioSign sense = RHSs[i].getRatioSign();
		if (LHSs[i].size()==0) {
#ifdef SHOW_EXTERN_SOLVER_WARNINGS
			std::cerr << "Preprocessing- or input - error in extern Solver." << std::endl;
#endif
			assert(RHSs[i].getValue().asDouble() >= -1e-9);
			continue;
		}
		double normer = std::fabs(LHSs[i][0].value.asDouble());
		double sign_factor = 1.0;
		if (sense == data::QpRhs::RatioSign::smallerThanOrEqual) {

		} else if (sense == data::QpRhs::RatioSign::greaterThanOrEqual) {
			//RHSs[i].setRatioSign(data::QpRhs::RatioSign::smallerThanOrEqual);
			//RHSs[i].setValue(-RHSs[i].getValue().asDouble());
			//sign_factor = -1.0;
		} else if (sense == data::QpRhs::RatioSign::equal) {
			//if (RHSs[i].getValue().asDouble() < 0 ) {
			//	RHSs[i].setValue(-RHSs[i].getValue().asDouble());
			//	sign_factor = -1.0;
			//}
		}
		//do not norm RHS!!!! RHSs[i].setValue(RHSs[i].getValue().asDouble() / normer);
		for (int j = 0; j < LHSs[i].size();j++) {
			data::IndexedElement elem;
			elem.value = sign_factor * LHSs[i][j].value.asDouble() / normer;
			elem.index = i;//LHSs[i][j].index;
			COLs[LHSs[i][j].index].push_back(elem);
			//COLs[LHSs[i][j].index][j].index = i;
		}
	}
	for (int i = 0; i < COLs.size();i++)
		sort( COLs[i].begin(), COLs[i].end(), []( data::IndexedElement p1, data::IndexedElement p2 ){ return p1.index < p2.index; } );
}

std::vector<data::IndexedElement>* idol::impl::QpExternSolverWithSnapshot::getRowLhs_snapshot(unsigned ri) {
	//lhs.clear();
	if (ri >= RHSs.size()) {
#ifdef SHOW_EXTERN_SOLVER_WARNINGS
		std::cerr << "Warning Extern Solver: row index = " << ri << ", but max index " << RHSs.size()-1 << std::endl;
#endif
		return 0;
	}
	return &LHSs[ri];
	//int rowsize=LHSs[ri].size();
	//for (int j = 0; j < rowsize;j++) lhs.push_back(LHSs[ri][j]);
}
