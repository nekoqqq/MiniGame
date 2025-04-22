#include "Selection.h"
#include "../Console/DDS.h"
#include "RootState.h"
#include "GameLib/Framework.h"

SelectionState::SelectionState()
{
    selection_img = new DDS("C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\img\\selection.dds");
}
SelectionState::~SelectionState()
{
    DYNAMIC_DEL(selection_img);
}

void SelectionState::update(RootState* parent) {
    for (char c : "123456789") {
        if (GameLib::Framework::instance().isKeyTriggered(c)) {
            parent->setStage(c - '0');
            parent->setState(RootState::MAIN);
        }
    }
    selection_img->drawImage();
}
