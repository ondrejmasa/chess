#include "uiElements.h"

void UIElement::HandleEvent(const SDL_Event &aEvent)
{
    if (aEvent.type == SDL_EVENT_MOUSE_BUTTON_UP)
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
    SDL_SetRenderDrawBlendMode(aSDLRenderer, SDL_BLENDMODE_BLEND);
    float off_x = 0.05f*Width;
    float off_y = 0.05f*Height;
    roundedBoxRGBA(aSDLRenderer, Left-off_x, Top-off_y, Left+Width+off_x, Top+Height+off_y, 15, 10, 15, 25, 150);
    TextureUIElement::Render(aSDLRenderer);
    if (IsSelected)
    {
        roundedRectangleRGBA(aSDLRenderer, Left-off_x, Top-off_y, Left+Width+off_x, Top+Height+off_y, 15, 255, 0, 0, 255);
    }
}

void TextureUIElement::Render(SDL_Renderer *aSDLRenderer)
{
    SDL_FRect r {Left, Top, Width, Height};
    SDL_RenderTexture(aSDLRenderer, mTexture, nullptr, &r);    
}

void ButtonUIElement::Render(SDL_Renderer *aSDLRenderer)
{
    TTF_SetFontSize(mFont, Height);
    SDL_Color col {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(mFont, mText.c_str(), 0, col);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(aSDLRenderer, textSurface);
    SDL_DestroySurface(textSurface); 

    float textWidth = 0.0f;
    float textHeight = 0.0f;
    SDL_GetTextureSize(textTexture, &textWidth, &textHeight);
    
    int w = Width;
    int h = Height;
    roundedBoxRGBA(aSDLRenderer, Left, Top, Left+w, Top+h, 5, 10, 15, 25, 180); 
    roundedRectangleRGBA(aSDLRenderer, Left, Top, Left+w, Top+h, 5, 0, 10, 30, 255);
    
    SDL_FRect renderQuad;
    renderQuad.x = Left + (w - textWidth) / 2.f;
    renderQuad.y = Top + (h - textHeight) / 2.f;
    renderQuad.w = textWidth;
    renderQuad.h = textHeight;
    SDL_RenderTexture(aSDLRenderer, textTexture, nullptr, &renderQuad);
    SDL_DestroyTexture(textTexture);       
}
