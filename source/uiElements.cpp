#include "uiElements.h"

void UIElement::HandleEvent(const SDL_Event &aEvent)
{
    if (aEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (aEvent.button.button == SDL_BUTTON_LEFT)
        {
            float mouseX = aEvent.button.x;
            float mouseY = aEvent.button.y;
            bool isInside = (mouseX >= Left && mouseX <= Left + Width) &&
                            (mouseY >= Top  && mouseY <= Top + Height);
            if (isInside)
            {
                if (OnClick)
                {
                    OnClick();
                }
            }
        }
    }
}

void SelectableUIElement::Render(SDL_Renderer *aSDLRenderer)
{
    float off_x = 0.05f*Width;
    float off_y = 0.05f*Height;
    roundedBoxRGBA(aSDLRenderer, Left-off_x, Top-off_y, Left+Width+off_x, Top+Height+off_y, 25, 10, 15, 25, 255);
    TextureUIElement::Render(aSDLRenderer);
    if (IsSelected)
    {
        roundedRectangleRGBA(aSDLRenderer, Left-off_x, Top-off_y, Left+Width+off_x, Top+Height+off_y, 25, 255, 0, 0, 255);
    }
}

void TextureUIElement::Render(SDL_Renderer *aSDLRenderer)
{
    SDL_FRect r {Left, Top, Width, Height};
    SDL_RenderTexture(aSDLRenderer, mTexture, nullptr, &r);    
}
