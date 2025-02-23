
// Else "mouse" component
struct MouseComponent : public GUIComponent
{
    typedef struct _mouse
    {
        t_object x_obj;
        int x_hzero;
        int x_vzero;
        int x_zero;
        int x_wx;
        int x_wy;
        t_glist* x_glist;
        t_outlet* x_horizontal;
        t_outlet* x_vertical;
    } t_mouse;
    
    MouseComponent(const pd::Gui& gui, Box* box, bool newObject) : GUIComponent(gui, box, newObject)
    {
        Desktop::getInstance().addGlobalMouseListener(this);
    }
    
    ~MouseComponent()
    {
        Desktop::getInstance().removeGlobalMouseListener(this);
    }
    
    void mouseMove(const MouseEvent& e) override
    {
        // Do this with a mouselistener?
        auto pos = Desktop::getInstance().getMousePosition();
        
        if (Desktop::getInstance().getMouseSource(0)->isDragging())
        {
            t_atom args[1];
            SETFLOAT(args, 0);
            
            pd_typedmess((t_pd*)gui.getPointer(), gensym("_up"), 1, args);
        }
        else
        {
            t_atom args[1];
            SETFLOAT(args, 1);
            
            pd_typedmess((t_pd*)gui.getPointer(), gensym("_up"), 1, args);
        }
        
        t_atom args[2];
        SETFLOAT(args, pos.x);
        SETFLOAT(args + 1, pos.y);
        
        pd_typedmess((t_pd*)gui.getPointer(), gensym("_getscreen"), 2, args);
    }
    
    bool noGui() override
    {
        return true;
    }
};
