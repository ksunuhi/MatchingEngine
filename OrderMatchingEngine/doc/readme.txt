
Design considerations
1, When matching order on the board, engine does not prohibit matching orders comes from the same paticipant

2, Maching engine's processing ability can be tuned by setNumberOfBoards(int), default number is 5.
it is the the number of matching order board, behind each matching board, there is 1 thread to work on the request.
matching board is orignized by Instrument code, orders with same asset go to the same board.

3, To avoid blocking user actions, a message queue is setup within matching engine.
newOrder() and cancelCancel() may takes time, so their requests go to the message queue, 
which are picked up by matching engine, processed asynchronously
getResultsOfLastOperation() is simple hash search and processed  synchronously

4, There are 3 test cases written in test.cpp, which should have convered all functionalities test
and give a demo the usage of Matching engine.

e.g a simple example
MatchingEngine& engine = MatchingEngine::getInstance();
Order order1("order1", 6759 ,OrderSide_t::sell_e,101,1000); //sell sony 1000 shares at price 101
engine.submitNew(order1); //submit to engine
engine.submitCancel(order1); //cancel the order
OrderEvent event = engine.getResultsOfLastOperation(order1); //the order's lastes status are returned in event

5, a class diagram is in the same folder with this file.