# include <Siv3D.hpp>

enum class BulletType
{
    Normal,
    Throw,
    Fall,
    FallThrow,
};

class Bullet
{
public:
    Bullet(bool isEnemy = false, Vec2 pos = Vec2(0,0), double speed=10.0, BulletType bulletType=BulletType::Normal)
    :m_isEnemy(isEnemy)
    ,m_pos(pos)
    ,m_speed(speed)
    ,m_prevPosY(pos.y)
    ,m_fouce(-12.0)
    ,m_bulletType(bulletType)
    ,m_isAlive(true)
    {}
    
    const bool alive()
    {
        return m_isAlive;
    }
    
    const Vec2 getPos()
    {
        return m_pos;
    }
    
    void dead()
    {
        m_isAlive = false;
    }
    
    void update()
    {
        if(m_isAlive)
        {
            double yTemp = m_pos.y;
            
            switch (m_bulletType)
            {
                case BulletType::Normal:
                    m_pos.moveBy(m_isEnemy ? -m_speed : m_speed, 0);
                    break;
                case BulletType::Throw:
                    m_pos.moveBy(m_isEnemy ? -m_speed : m_speed, (m_pos.y - m_prevPosY) + m_fouce);
                    m_prevPosY = yTemp;
                    m_fouce = 0.2;
                    break;
                case BulletType::Fall:
                    m_pos.moveBy(0, m_speed);
                    break;
                case BulletType::FallThrow:
                    m_pos.moveBy(m_isEnemy ? -m_speed/2 : m_speed/2, m_speed);
                    break;
                default:
                    m_pos.moveBy(m_isEnemy ? -m_speed : m_speed, 0);
                    break;
            }
            
        }
    }
    
    const void draw()
    {
        if(m_isAlive) RectF(m_pos-Vec2(20,20),40,40).rotated(System::FrameCount()*1.0_deg).draw(m_isEnemy ? Palette::Blue : Palette::Red);
    }
private:
    const bool m_isEnemy;
    const BulletType m_bulletType;
    Vec2 m_pos;
    bool m_isAlive;
    double m_speed;
    double m_fouce;
    double m_prevPosY;
};

class Player
{
public:
    Player(String name=U"打", int32 grade=1, bool isEnemy = false, Vec2 pos = Vec2(0,0))
    :m_font(100,Typeface::Bold)
    ,m_name(name)
    ,m_grade(grade)
    ,m_isEnemy(isEnemy)
    ,m_pos(Vec2(pos.x,0))
    ,m_fallPos(pos.y)
    ,m_isAlive(true)
    {
        if(m_name==U"打")
        {
            m_speed = isEnemy ? -2.0 : 2.0;
            m_hp = 10+5*grade;
        }
        else if(m_name==U"撃")
        {
            m_speed = isEnemy ? -1.5 : 1.5;
            m_hp = 10;
        }
        else if(m_name==U"射")
        {
            m_speed = isEnemy ? -1.0 : 1.0;
            m_hp = 5;
        }
        else if(m_name==U"伐")
        {
            m_speed = grade + (isEnemy ? -5.0 : 5.0);
            m_hp = 25;
        }
        else if(m_name==U"征")
        {
            m_speed = isEnemy ? -3.0 : 3.0;
            m_hp = 5;
            m_fallPos -= 200;
        }
        
        m_coolTime.start();
    }
    
    const bool alive()
    {
        return m_isAlive;
    }
    
    const Vec2 getPos()
    {
        return m_pos;
    }
    
    Array<Bullet>& getBullets()
    {
        return m_bullets;
    }
    
    int32 getGrade()
    {
        return m_grade;
    }
    
    bool nockBack(int32 damage)
    {
        m_hp -= damage;
        if(m_hp <= 0)
        {
            m_isAlive = false;
            return true;
        }
        m_pos.moveBy(-m_speed*10, 0);
        return false;
    }
    
    void dead()
    {
        m_isAlive = false;
    }
    
    void update()
    {
        if(m_isAlive)
        {
            if(m_name==U"撃")
            {
                if(2000/m_grade < m_coolTime.ms())
                {
                    m_coolTime.restart();
                    m_bullets.push_back(Bullet(m_isEnemy,m_pos,10.0,BulletType::Normal));
                }
            }
            else if(m_name==U"射")
            {
                if(4000 < m_coolTime.ms())
                {
                    m_coolTime.restart();
                    m_bullets.push_back(Bullet(m_isEnemy,m_pos,3.0,BulletType::Throw));
                    m_bullets.push_back(Bullet(m_isEnemy,m_pos,6.0,BulletType::Throw));
                    m_bullets.push_back(Bullet(m_isEnemy,m_pos,9.0,BulletType::Throw));
                    if(2 <= m_grade)
                    {
                        m_bullets.push_back(Bullet(m_isEnemy,m_pos,1.0,BulletType::Throw));
                    }
                    if(3 <= m_grade)
                    {
                        m_bullets.push_back(Bullet(m_isEnemy,m_pos,12.0,BulletType::Throw));
                    }
                }
            }
            else if(m_name==U"征")
            {
                if(500 < m_coolTime.ms())
                {
                    m_coolTime.restart();
                    m_bullets.push_back(Bullet(m_isEnemy,m_pos,10.0,BulletType::Fall));
                    if(2 <= m_grade)
                    {
                        m_bullets.push_back(Bullet(m_isEnemy,m_pos,10.0,BulletType::FallThrow));
                    }
                    if(3 <= m_grade)
                    {
                        m_bullets.push_back(Bullet(!m_isEnemy,m_pos,10.0,BulletType::FallThrow));
                    }
                }
            }
        
            if(m_fallPos > m_pos.y)
            {
                m_pos.moveBy(m_speed, 10.0);
            }
            else
            {
                m_pos.moveBy(m_speed, 0);
            }
                
        }
        
        for(auto& bullet : m_bullets)
        {
            bullet.update();
        }
    }
    
    const void draw()
    {
        if(m_isAlive)
        {
            switch (m_grade)
            {
                case 1:
                    break;
                
                case 2:
                    m_font(U"激").drawAt(m_pos-Vec2(0,100), m_isEnemy ? Palette::Blue : Palette::Red);
                    break;
                
                case 3:
                    m_font(U"超").drawAt(m_pos-Vec2(0,100), m_isEnemy ? Palette::Blue : Palette::Red);
                    break;
                
                default:
                    break;
            }
            
            m_font(m_name).drawAt(m_pos, m_isEnemy ? Palette::Blue : Palette::Red);
            
        }
        
        for(auto& bullet : m_bullets)
        {
            bullet.draw();
        }
    }
private:
    const Font m_font;
    const String m_name;
    const bool m_isEnemy;
    const int32 m_grade;
    double m_speed;
    Vec2 m_pos;
    int32 m_hp;
    bool m_isAlive;
    Stopwatch m_coolTime;
    Array<Bullet> m_bullets;
    double m_fallPos;
};

struct Default : IEffect
{
    struct Particle
    {
        Vec2 pos, v0;
    };
    
    Array<Particle> m_particles;
    
    Default(const Vec2& pos, int count)
    : m_particles(count)
    {
        int i=0;
        for (auto& particle : m_particles)
        {
            const Vec2 v = Circular(10, 360_deg/count*i);
            particle.pos = pos + Vec2(0,-50) + v;
            particle.v0 = v * 20.0;
            ++i;
        }
    }
    
    bool update(double t) override
    {
        for (const auto& particle : m_particles)
        {
            const Vec2 pos = particle.pos + particle.v0 * t + 0.5* t*t * Vec2(0, 640);
            
            Circle(pos,10).draw(ColorF(HSV(360*t),1.0 - t));
        }
        
        return t < 1.0;
    }
};

struct Fall : IEffect
{
    struct Particle
    {
        Vec2 pos, v0;
    };
    
    Array<Particle> m_particles;
    Color m_color;
    
    Fall(const Vec2& pos, int count, Color color)
    : m_particles(count)
    , m_color(color)
    {
        int i=0;
        for (auto& particle : m_particles)
        {
            const Vec2 v = Circular(10, 360_deg/count*i);
            particle.pos = pos + Vec2(0,0) + v;
            particle.v0 = v * 10.0;
            ++i;
        }
    }
    
    bool update(double t) override
    {
        for (const auto& particle : m_particles)
        {
            const Vec2 pos = particle.pos + particle.v0 * t + 0.5* t*t * Vec2(0, 0);
            
            RectF(pos,30,30).rotated(t*360_deg).draw(ColorF(m_color,1.0 - t));
        }
        
        return t < 1.0;
    }
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
    
    Stopwatch gameCount;
    
    Effect effect;
    bool isGameOver = false;
    bool isStart = false;
    
    const Font UIFont(40,Typeface::Bold);
    const Font font(80,Typeface::Bold);
    const Font bigFont(250,Typeface::Bold);
    const Font powerUpFont(60,Typeface::Bold);
    Array<HighlightingShape<Rect>> items;
    Array<String> itemNames = {U"撃",U"打",U"伐",U"射",U"征"};
    Array<int32> itemEnergies = {200,200,500,500,1000};
    Array<int32> itemNumber = {0,0,0,0,0};
    const int32 itemCount = 5;
    const Vec2 itemRange(100,50);
    const Vec2 itemSize((Window::Size().x)/itemCount-itemRange.x, Window::Size().y/5);
    
    Stopwatch coolTime;
    int32 nextCoolTime = 0;
    Array<Player> players;
    Stopwatch respawnTime;
    Array<Player> enemies;
    
    int32 score = 0;
    int32 energy = 2500;
    const int32 maxEnergy = 100000;
    int32 deadCount = 0;
    const int32 maxDeadCount = 5;
    
    Audio bgm(U"example/bgm_maoudamashii_8bit25.mp3", Arg::loop_<bool>(true));
    Audio selectSE(U"example/Pickup_Coin62.wav");
    Audio cancelSE(U"example/Laser_Shoot58.wav");
    Audio deadSE(U"example/Explosion69.wav");
    Audio damageSE(U"example/Explosion72.wav");
    Audio deathSE(U"example/Explosion78.wav");
    Audio powerUpSE(U"example/Explosion31.wav");
    
    for (auto i : step(itemCount))
    {
        items << HighlightingShape<Rect>(itemRange.x+i*itemSize.x*1.5, Window::Size().y-itemSize.y-itemRange.y, itemSize.x, itemSize.y);
    }
    
    while (System::Update())
    {
        if(!isStart && MouseL.down())
        {
            coolTime.start();
            gameCount.start();
            respawnTime.start();
            
            selectSE.playOneShot();
            bgm.play();
            isStart = true;
        }
        
        
        if(isStart && energy < maxEnergy)
        {
            energy += (1+itemNumber.sum()/(itemCount*2));
        }
        
        if(!isGameOver && nextCoolTime < respawnTime.s())
        {
            respawnTime.restart();
            
            if(gameCount.s() < 75)
            {
                nextCoolTime = Random(1,4);
            }
            else
            {
                nextCoolTime = 1;
            }
            
            int32 grade = 1;
            
            if(45 < gameCount.s())
            {
                grade = 2;
            }
            
            if(90 < gameCount.s())
            {
                grade = 3;
            }
            
            int32 n = Random(0,100);
            if(n < 40)
            {
                enemies.push_back(Player(itemNames[0],grade,true,Vec2(Window::Size().x+50,Window::Size().y/2+100)));
            }
            else if(n < 60)
            {
                enemies.push_back(Player(itemNames[1],grade,true,Vec2(Window::Size().x+50,Window::Size().y/2+100)));
            }
            else if(n < 80)
            {
                enemies.push_back(Player(itemNames[2],grade,true,Vec2(Window::Size().x+50,Window::Size().y/2+100)));
            }
            else if(n < 90)
            {
                enemies.push_back(Player(itemNames[3],grade,true,Vec2(Window::Size().x+50,Window::Size().y/2+100)));
            }
            else
            {
                enemies.push_back(Player(itemNames[4],grade,true,Vec2(Window::Size().x+50,Window::Size().y/2+100)));
            }
            
        }
        
        for (auto& i : step(items.size()))
        {
            items[i].update();
            if(!isGameOver && items[i].shapeClicked())
            {
                if(itemNumber[i]<10)
                {
                    if(itemEnergies[i] < energy && 500 < coolTime.ms())
                    {
                        ++itemNumber[i];
                        if(itemNumber[i]==10 || itemNumber[i]==25)
                        {
                            powerUpSE.playOneShot();
                        }
                        selectSE.playOneShot();
                        coolTime.restart();
                        energy -= itemEnergies[i];
                        players.push_back(Player(itemNames[i],1,false,Vec2(-50,Window::Size().y/2+100)));
                    }
                    else
                    {
                        cancelSE.playOneShot();
                    }
                }
                else if(itemNumber[i]<25)
                {
                    if(itemEnergies[i]*2 < energy && 1000 < coolTime.ms())
                    {
                        ++itemNumber[i];
                        selectSE.playOneShot();
                        coolTime.restart();
                        energy -= itemEnergies[i]*2;
                        players.push_back(Player(itemNames[i],2,false,Vec2(-50,Window::Size().y/2+100)));
                    }
                    else
                    {
                        cancelSE.playOneShot();
                    }
                }
                else
                {
                    if(itemEnergies[i]*3 < energy && 1000 < coolTime.ms())
                    {
                        ++itemNumber[i];
                        selectSE.playOneShot();
                        coolTime.restart();
                        energy -= itemEnergies[i]*3;
                        players.push_back(Player(itemNames[i],3,false,Vec2(-50,Window::Size().y/2+100)));
                    }
                    else
                    {
                        cancelSE.playOneShot();
                    }
                }
            }
        }
        
        for(auto& player : players)
        {
            for(auto& enemy : enemies)
            {
                if(Circle(player.getPos(),30).intersects(Circle(enemy.getPos(),30)))
                {
                    if(player.alive() && enemy.alive())
                    {
                        effect.add<Default>(Vec2((player.getPos().x+enemy.getPos().x)/2,player.getPos().y),10);
                        
                        if(player.nockBack(5*enemy.getGrade()))
                        {
                            effect.add<Fall>(player.getPos(), 10, Palette::Red);
                            deadSE.playOneShot();
                        }
                        if(enemy.nockBack(5*player.getGrade()))
                        {
                            effect.add<Fall>(enemy.getPos(), 10, Palette::Blue);
                            deadSE.playOneShot();
                            score += 100;
                        }
                        else
                        {
                            damageSE.playOneShot();
                        }

                    }
                }
                
                for(auto& pBullet : player.getBullets())
                {
                    if(Circle(pBullet.getPos(),20).intersects(Circle(enemy.getPos(),30)))
                    {
                        if(pBullet.alive() && enemy.alive())
                        {
                            effect.add<Default>(Vec2((pBullet.getPos().x+enemy.getPos().x)/2,enemy.getPos().y),6);
                            pBullet.dead();
                            
                            if(enemy.nockBack(2))
                            {
                                effect.add<Fall>(enemy.getPos(), 10, Palette::Blue);
                                deadSE.playOneShot();
                                score += 100;
                            }
                            else
                            {
                                damageSE.playOneShot();
                            }

                        }
                    }
                }
                
                for(auto& eBullet : enemy.getBullets())
                {
                    if(Circle(eBullet.getPos(),20).intersects(Circle(player.getPos(),30)))
                    {
                        if(eBullet.alive() && player.alive())
                        {
                            effect.add<Default>(Vec2((eBullet.getPos().x+player.getPos().x)/2,player.getPos().y),6);
                            eBullet.dead();
                            
                            if(player.nockBack(2))
                            {
                                effect.add<Fall>(player.getPos(), 10, Palette::Blue);
                                deadSE.playOneShot();
                            }
                            else
                            {
                                damageSE.playOneShot();
                            }
                        }
                    }
                }
            }
        }
        
        for(auto& player : players)
        {
            player.update();
            
            if(player.alive() && Window::Size().x+100 < player.getPos().x)
            {
                player.dead();
                powerUpSE.playOneShot();
                score += 1000;
            }
            
            for(auto& pBullet : player.getBullets())
            {
                if(pBullet.alive() && Window::Size().x+50 < pBullet.getPos().x)
                {
                    pBullet.dead();
                }
            }
        }
        
        for(auto& enemy : enemies)
        {
            enemy.update();
            
            if(enemy.alive() && enemy.getPos().x < -100)
            {
                ++deadCount;
                deadSE.playOneShot();
                if(maxDeadCount <= deadCount)
                {
                    isGameOver = true;
                }
                enemy.dead();
            }
            
            for(auto& eBullet : enemy.getBullets())
            {
                if(eBullet.alive() && eBullet.getPos().x < -50)
                {
                    eBullet.dead();
                }
            }
        }

        if(Rect(0,200,100,Window::Size().y-450).leftClicked() &&  5000 < score)
        {
            score -= 5000;
            
            deadSE.playOneShot();
            for(const auto& i : step(10))
            {
                 effect.add<Default>(Vec2(200+i*150,Window::Size().y/2-200), 20);
            }
            
            for(auto& enemy : enemies)
            {
                for(auto& eBullet : enemy.getBullets())
                {
                    eBullet.dead();
                }
                enemy.dead();
            }
        }
        /*
        auto rmvIter = std::remove_if(players.begin(),players.end(),[](Player& p){return 500.0<p.getPos().x;});
        players.erase(rmvIter, players.end());
        */
        //draw
        for(auto& player : players)
        {
            player.draw();
        }
        
        for(auto& enemy : enemies)
        {
            enemy.draw();
        }
        
        effect.update();
        
        for (const auto& item : items)
        {
            item.drawHighlight(Palette::Gray);
        }
        
        for (const auto& i : step(itemCount))
        {
            if(itemNumber[i]<10)
            {
                font(itemNames[i]).draw(itemRange.x+30+i*itemSize.x*1.5, Window::Size().y-itemSize.y,Palette::Gray);
                UIFont(itemEnergies[i]).draw(Arg::topRight(itemRange.x+150+i*itemSize.x*1.5, Window::Size().y-itemSize.y-itemRange.y),Palette::Gray);
            }
            else if(itemNumber[i]<25)
            {
                powerUpFont(U"激",itemNames[i]).draw(itemRange.x+i*itemSize.x*1.5, Window::Size().y-itemSize.y+20,Palette::Gray);
                UIFont(itemEnergies[i]*2).draw(Arg::topRight(itemRange.x+150+i*itemSize.x*1.5, Window::Size().y-itemSize.y-itemRange.y),Palette::Gray);
            }
            else
            {
                powerUpFont(U"超",itemNames[i]).draw(itemRange.x+i*itemSize.x*1.5, Window::Size().y-itemSize.y+20,Palette::Gray);
                UIFont(itemEnergies[i]*3).draw(Arg::topRight(itemRange.x+150+i*itemSize.x*1.5, Window::Size().y-itemSize.y-itemRange.y),Palette::Gray);
            }
        }
        bigFont(U"鬱").drawAt(0,Window::Size().y/2,Palette::Gray);
        
        UIFont(U"Score : ").draw(50,0,Palette::Gray);
        UIFont(score).draw(Arg::topRight(Window::Size().x/2-50, 0),Palette::Gray);
        
        UIFont(U"Energy : ").draw(50+Window::Size().x/2,0,Palette::Gray);
        UIFont(energy).draw(Arg::topRight(Window::Size().x-50, 0),Palette::Gray);
        
        for (const auto& i : step(maxDeadCount))
        {
            if(i<deadCount)
            {
                UIFont(U"鬱").draw(50+i*50,80,Palette::Blue);
            }
            else
            {
                UIFont(U"鬱").draw(50+i*50,80,ColorF(Palette::Gray,0.5));
            }
        }
        
        if(!isStart)
        {
            font(U"マウスクリックでスタート").drawAt(Window::Center(),Palette::Red);
        }
        
        if(isGameOver)
        {
            bgm.stop();
            font(U"GameOver").drawAt(Window::Center(),Palette::Red);
        }
    }
}
