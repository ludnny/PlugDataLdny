#pragma once

#include <JuceHeader.h>
#include <m_pd.h>
#include "Edge.h"
#include "Pd/PdObject.hpp"
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class Canvas;
class Connection  : public Component, public ComponentListener
{
public:
    
    SafePointer<Edge> start, end;
    Path path;
    
    Canvas* cnv;
    
    int inIdx;
    int outIdx;
    pd::Object* inObj;
    pd::Object* outObj;
    
    bool isSelected = false;
    
    //==============================================================================
    Connection(Canvas* parent, Edge* start, Edge* end, bool exists = false);
    ~Connection() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void mouseDown(const MouseEvent& e) override;
    
    void deleteListeners();
    
    void componentMovedOrResized (Component &component, bool wasMoved, bool wasResized) override;
    
    virtual void componentBeingDeleted(Component& component) override;

private:
    
    bool deleted = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Connection)
};
