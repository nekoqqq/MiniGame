#include "Selection.h"
#include "../Console/DDS.h"
#include "RootState.h"
#include "GameLib/Framework.h"
#include "StringDrawer.h"
#include <sstream>
using GameLib::Framework;
SelectionState::SelectionState() :cur_stage(1)
{
    selection_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\selection.dds");
}
SelectionState::~SelectionState()
{
    DYNAMIC_DEL(selection_img);
}

void SelectionState::update(RootState* parent) {
    if (Framework::instance().isKeyTriggered('w') || Framework::instance().isKeyTriggered('W')) {
        cur_stage = (cur_stage+7) % 9 + 1;
    }
    else if (Framework::instance().isKeyTriggered('s') || Framework::instance().isKeyTriggered('S'))
        cur_stage = cur_stage % 9 + 1;
    else if (Framework::instance().isKeyTriggered(' ')) {
        parent->setStage(cur_stage);
        parent->setState(RootState::MAIN);
    }


    selection_img->drawImage();
    std::ostringstream oss;
    for (int i = 0; i <= 9; i++) {
        if (i == 0) {
            oss << "Press SPACE button";
            StringDrawer::instance().drawStringAt(i, 0, oss.str().c_str(), 0xffffff);
        }
        else if (i != cur_stage) {
            oss << "    stage " << i << std::endl;
            StringDrawer::instance().drawStringAt(i, 0, oss.str().c_str(), 0x00ff00);
        }
        else {
            oss << "  :>stage " << i << std::endl;
            StringDrawer::instance().drawStringAt(i, 0, oss.str().c_str(), 0xff0000);
        }
        oss.str("");
    }
}
