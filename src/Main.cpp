# include <Siv3D.hpp>

class Player
{
public:
    Player(String name=U"打", double speed=1.0, Vec2 pos = Vec2(0,0))
    :m_font(100,Typeface::Bold)
    ,m_name(name)
    ,m_speed(speed)
    ,m_pos(pos)
    {}
    
    Vec2 getPos()
    {
        return m_pos;
    }
    
    void update()
    {
        m_pos.moveBy(m_speed, 0);
    }
    
    const void draw()
    {
        m_font(m_name).drawAt(m_pos, Palette::Gray);
    }
private:
    const Font m_font;
    const String m_name;
    const double m_speed;
    Vec2 m_pos;
};

template <class ShapeType>
class HighlightingShape : public ShapeType
{
private:
    
    Transition m_transition = Transition(0.2s, 0.1s);
    
public:
    
    HighlightingShape() = default;
    
    explicit HighlightingShape(const ShapeType& shape)
    : ShapeType(shape) {}
    
    template <class ...Args>
    explicit HighlightingShape(Args&&... args)
    : ShapeType(std::forward<Args>(args)...) {}
    
    void update()
    {
        m_transition.update(ShapeType::mouseOver());
    }
    
    void drawHighlight(Color color) const
    {
        ShapeType::drawFrame(0, 10, color);
        
        ShapeType::draw(ColorF(color, m_transition.value() * 0.25));
    }
    
    const bool shapeClicked()
    {
        return ShapeType::leftClicked();
    }
    
    const Transition& getTransition() const
    {
        return m_transition;
    }
};

void Main()
{
    Window::Resize(1280, 720);
    Graphics::SetBackground(Palette::Whitesmoke);
    
    const Font UIFont(40,Typeface::Bold);
    const Font font(80);
    Array<HighlightingShape<Rect>> items;
    Array<String> itemNames = {U"打",U"射",U"撃",U"伐",U"征"};
    Array<int32> itemEnergies = {100,500,1000,2000,5000};
    const int32 itemCount = 5;
    const Vec2 itemRange(100,50);
    const Vec2 itemSize((Window::Size().x)/itemCount-itemRange.x, Window::Size().y/5);
    
    Stopwatch stopWatch;
    Array<Player> players;
    
    int32 score = 0;
    int32 energy = 0;
    const int32 maxEnergy = 100000;
    int32 deadCount = 0;
    const int32 maxDeadCount = 5;
    
    for (auto i : step(itemCount))
    {
        items << HighlightingShape<Rect>(itemRange.x+i*itemSize.x*1.5, Window::Size().y-itemSize.y-itemRange.y, itemSize.x, itemSize.y);
    }
    
    stopWatch.start();
    
    while (System::Update())
    {
        if(energy < maxEnergy)
        {
            ++energy;
        }
        
        for (auto& i : step(items.size()))
        {
            items[i].update();
            if(items[i].shapeClicked())
            {
                if(itemEnergies[i] < energy && 2000 < stopWatch.ms())
                {
                    stopWatch.restart();
                    energy -= itemEnergies[i];
                    players.push_back(Player(itemNames[i],1.0,Vec2(-100,Window::Size().y/2+100)));
                }
            }
        }
        
        for(auto& player : players)
        {
            player.update();
        }
        
        //draw
        for (const auto& item : items)
        {
            item.drawHighlight(Palette::Gray);
        }
        
        for (const auto& i : step(itemCount))
        {
            font(itemNames[i]).draw(itemRange.x+30+i*itemSize.x*1.5, Window::Size().y-itemSize.y,Palette::Gray);
            
            UIFont(itemEnergies[i]).draw(Arg::topRight(itemRange.x+150+i*itemSize.x*1.5, Window::Size().y-itemSize.y-itemRange.y),Palette::Gray);
        }
        
        for(auto& player : players)
        {
            player.draw();
        }
        
        
        UIFont(U"Score : ").draw(50,0,Palette::Gray);
        UIFont(score).draw(Arg::topRight(Window::Size().x/2-50, 0),Palette::Gray);
        
        UIFont(U"Energy : ").draw(50+Window::Size().x/2,0,Palette::Gray);
        UIFont(energy).draw(Arg::topRight(Window::Size().x-50, 0),Palette::Gray);
        
        for (const auto& i : step(maxDeadCount))
        {
            if(i<deadCount)
            {
                UIFont(U"鬱").draw(50+i*50,80,Palette::Gray);
            }
            else
            {
                UIFont(U"鬱").draw(50+i*50,80,ColorF(Palette::Gray,0.5));
            }
        }
    }
}
