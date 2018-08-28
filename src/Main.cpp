# include <Siv3D.hpp>

void Main()
{
    while (System::Update())
    {
        if(Gamepad(0))
        {
            auto gamePad = Gamepad(0);
            
            for (size_t i = 0; i < gamePad.buttons.size(); ++i)
            {
                if (gamePad.buttons[i].down())
                {
                    Print(i);
                }
            }
            
            Print(gamePad.povD8());
        }
    }
}
