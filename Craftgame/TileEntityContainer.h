#pragma once
#include "TileEntity.h"
#include "InventoryContainer.h"
#include "CSInterface.h"

class TileEntityContainer: public TileEntity
{
private:
	InventoryContainer* inv = nullptr;
	CSInterface* interface;
public:

	TileEntityContainer(Block* t);
	InventoryContainer* getInventory();
	virtual ~TileEntityContainer() override;
	CSInterface* getInterface();


	virtual boost::shared_ptr<ADT::Map> serialize();
	virtual void deserialize(boost::shared_ptr<ADT::Map> data);
};
