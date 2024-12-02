import veins.base.utils.Coord;
import veins.modules.messages.BaseFrame1609_4;
import veins.base.utils.SimpleAddress;
 
//namespace Veins;
 

packet BSMMessage{
    int type;
    double time;
    string LaneID;
	int VehicleID=0;
	double speed;
	double acl;
	double posX;
	double posY;
}