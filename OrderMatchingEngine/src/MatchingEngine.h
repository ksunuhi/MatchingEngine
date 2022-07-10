/*
 * MatchingEngine.h
 *
 *  Created on: 2022Äê7ÔÂ9ÈÕ
 *      Author: yufei
 */

#ifndef MATCHINGENGINE_H_
#define MATCHINGENGINE_H_

#include <vector>

#include "Order.h"
#include "OrderEvent.h"
#include "OrderBoard.h"

namespace ENGINE {

/**
 * A singleton class to represent order matching engine
 */
class MatchingEngine {
public:
	static void setNumberOfBoards(int boards);
	static MatchingEngine& getInstance();

	MatchingEngine(const MatchingEngine& ) = delete;
	void operator = (const MatchingEngine& ) = delete;
	virtual ~MatchingEngine() = default;

	/**
	 * A async call to submit new order to Engine
	 * [in] order - clientId,instrument,side,price,quantity must be provided.
	 *              otherwise, valiation fails
	 * return
	 *    false:validaton fails, true:validation ok
	 */
	bool submitNew(const Order& order);

	/**
	 * A async call to submit new order to Engine
	 * Param:
	 *    [in] order - clientId and instrument must be provided.
	 *                 otherwise, valiation fails
	 * return
	 *    false-validaton fails true:validation ok
	 */
	bool submitCancel(const Order& order);

	/**
	 * A async call to submit new order to Engine
	 * Param:
	 *    [in] order - clientId and instrument must be provided.
	 *                 otherwise, valiation fails
	 * return
	 *    OrderEvent- when fails to find out specified, its status is unknown_e
	 */
	OrderEvent getResultsOfLastOperation(const Order& order);

private:
	MatchingEngine();
	bool validationNew(const Order& order);
	bool validationCancel(const Order& order);

private:
	static int ourNumberOfBoards;
	std::vector<OrderBoard> myBoards; //split order by instrument on order boards

};

}

#endif /* MATCHINGENGINE_H_ */
