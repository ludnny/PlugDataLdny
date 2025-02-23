

struct ToggleComponent : public GUIComponent
{
    
    struct Toggle : public TextButton
    {
        std::function<void()> onMouseDown;
        
        void paint(Graphics& g) override
        {
            auto offColour = findColour(TextButton::buttonColourId).overlaidWith(Colours::grey.withAlpha(0.42f));
            
            g.setColour(getToggleState() ? findColour(TextButton::buttonOnColourId) : offColour);
            
            
            auto crossBounds = getLocalBounds().reduced(6).toFloat();
            
            if(getWidth() < 20) {
                crossBounds = crossBounds.expanded(20 - getWidth());
            }
            
            const auto max = std::max(crossBounds.getWidth(), crossBounds.getHeight());
            const auto strokeWidth = std::max(max * 0.15f, 2.0f);
            
            g.drawLine({crossBounds.getTopLeft(), crossBounds.getBottomRight()}, strokeWidth);
            g.drawLine({crossBounds.getBottomLeft(), crossBounds.getTopRight()}, strokeWidth);
        }
        
        void mouseDown(const MouseEvent& e) override {
            TextButton::mouseDown(e);
            onMouseDown();
        }
    };
    
    Toggle toggleButton;
    
    ToggleComponent(const pd::Gui& pdGui, Box* parent, bool newObject) : GUIComponent(pdGui, parent, newObject)
    {
        addAndMakeVisible(toggleButton);
        toggleButton.setToggleState(getValueOriginal(), dontSendNotification);
        toggleButton.setConnectedEdges(12);
        toggleButton.setName("pd:toggle");
        
        toggleButton.onMouseDown = [this]()
        {
            startEdition();
            auto newValue = 1.f - getValueOriginal();
            setValueOriginal(newValue);
            toggleButton.setToggleState(newValue, dontSendNotification);
            stopEdition();
            
            update();
        };
        
        initialise(newObject);
    }
    
    void checkBoxBounds() override {
        // Fix aspect ratio and apply limits
        int size = jlimit(30, maxSize, box->getWidth());
        if(size != box->getHeight() || size != box->getWidth()) {
            box->setSize(size, size);
        }
    }
    
    
    void resized() override
    {
        toggleButton.setBounds(getLocalBounds());
    }
    
    void update() override
    {
        toggleButton.setToggleState((getValueOriginal() > std::numeric_limits<float>::epsilon()), dontSendNotification);
    }

};
