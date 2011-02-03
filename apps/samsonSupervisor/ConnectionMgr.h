#ifndef CONNECTION_MGR_H
#define CONNECTION_MGR_H

/* ****************************************************************************
*
* FILE                     ConnectionMgr.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
class DelilahScene;
class DelilahQueue;
class DelilahConnection;



/* ****************************************************************************
*
* ConnectionMgr
*/
class ConnectionMgr
{
public:
	ConnectionMgr(unsigned int size);
	void insert(DelilahScene* sceneP, DelilahQueue* from, DelilahQueue* to);
	void remove(DelilahQueue* queue);
	void move(DelilahQueue* queue);

private:
	DelilahConnection** conV;
	unsigned int        size;
};

#endif
