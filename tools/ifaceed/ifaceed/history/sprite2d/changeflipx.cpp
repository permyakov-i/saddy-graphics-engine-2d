#include "changeflipx.h"

// ReSharper disable once CppUnusedIncludeDirective
#include <db/save.h>


#include "../../gui/uiblocks/uiblocks.h"
#include "../../gui/uiblocks/uispriteblock.h"

history::Command* history::sprite2d::changeFlipX(
    core::Editor* e, 
    sad::SceneNode* d, 
    bool oldvalue, 
    bool newvalue
)
{
    return new history::scenenodes::ChangePropertyWhichLinkedToCheckbox(
        e->uiBlocks()->uiSpriteBlock()->cbFlipX,
        d,
        "flipx",
        oldvalue,
        newvalue
    );	
}
