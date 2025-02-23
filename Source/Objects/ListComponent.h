
struct ListComponent : public GUIComponent, public Timer
{
    ListComponent(const pd::Gui& gui, Box* parent, bool newObject) : GUIComponent(gui, parent, newObject)
    {
        label.setBounds(2, 0, getWidth() - 2, getHeight() - 1);
        label.setMinimumHorizontalScale(1.f);
        label.setJustificationType(Justification::centredLeft);
        label.setBorderSize(BorderSize<int>(2, 6, 2, 2));
        label.setText(String(getValueOriginal()), dontSendNotification);
        label.setEditable(true, false);
        
        setInterceptsMouseClicks(true, false);
        addAndMakeVisible(label);
        
        label.onEditorHide = [this]()
        {
            auto gui = getGui();
            auto array = StringArray();
            array.addTokens(label.getText(), true);
            std::vector<pd::Atom> list;
            list.reserve(array.size());
            for (auto const& elem : array)
            {
                if (elem.getCharPointer().isDigit())
                {
                    list.push_back({elem.getFloatValue()});
                }
                else
                {
                    list.push_back({elem.toStdString()});
                }
            }
            if (list != gui.getList())
            {
                startEdition();
                gui.setList(list);
                stopEdition();
                // label.setText(String(gui.getSymbol()), NotificationType::dontSendNotification);
            }
        };
        
        label.onEditorShow = [this]()
        {
            auto* editor = label.getCurrentTextEditor();
            if (editor != nullptr)
            {
                editor->setIndents(1, 2);
                editor->setBorder(BorderSize<int>(2, 6, 2, 2));
            }
        };
        
        updateValue();
        
        initialise(newObject);
        startTimer(100);
    }
    
    
    void checkBoxBounds() override {
        // Apply size limits
        int w = jlimit(100, maxSize, box->getWidth());
        int h = jlimit(Box::height - 2, maxSize, box->getHeight());
        
        if(w != box->getWidth() || h != box->getHeight()) {
            box->setSize(w, h);
        }
    }
    
    ~ListComponent()
    {
        stopTimer();
    }
    
    void timerCallback() override
    {
        update();
    }
    
    void resized() override
    {
        label.setBounds(getLocalBounds());
    }
    
    void paint(Graphics& g) override
    {
        getLookAndFeel().setColour(Label::textWhenEditingColourId, box->findColour(Label::textWhenEditingColourId));
        getLookAndFeel().setColour(Label::textColourId, box->findColour(Label::textColourId));
        
        g.fillAll(box->findColour(PlugDataColour::highlightColourId));
        
        static auto const border = 1.0f;
        const auto h = static_cast<float>(getHeight());
        const auto w = static_cast<float>(getWidth());
        const auto o = h * 0.25f;
        Path p;
        p.startNewSubPath(0.5f, 0.5f);
        p.lineTo(0.5f, h - 0.5f);
        p.lineTo(w - o, h - 0.5f);
        p.lineTo(w - 0.5f, h - o);
        p.lineTo(w - 0.5f, o);
        p.lineTo(w - o, 0.5f);
        p.closeSubPath();
        
        g.setColour(box->findColour(PlugDataColour::canvasColourId));
        g.fillPath(p);
        g.strokePath(p, PathStrokeType(border));
    }
    
    void update() override
    {
        if (!edited && !label.isBeingEdited())
        {
            auto const array = gui.getList();
            String message;
            for (auto const& atom : array)
            {
                if (message.isNotEmpty())
                {
                    message += " ";
                }
                if (atom.isFloat())
                {
                    message += String(atom.getFloat());
                }
                else if (atom.isSymbol())
                {
                    message += String(atom.getSymbol());
                }
            }
            label.setText(message, NotificationType::dontSendNotification);
        }
    }
    
private:
    Label label;
};
