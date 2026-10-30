# pragma once
#include <functional>
#include <SDL3/SDL.h>
#include <SDL3_gfx/SDL3_gfxPrimitives.h>


class UIElement {
public:
    UIElement(const float aLeft, const float aTop, const float aWidth, const float aHeight)
        :   Left {aLeft},
            Top {aTop},
            Width {aWidth},
            Height {aHeight} {}
    float Left, Top;
    float Width, Height;
    std::function<void()> OnClick;
    virtual void Render(SDL_Renderer* aSDLRenderer) = 0;
    void HandleEvent(const SDL_Event& aEvent); 
    void Update(const float aLeft, const float aTop, const float aWidth, const float aHeight)
    {
        Left = aLeft;
        Top = aTop;
        Width = aWidth;
        Height = aHeight;      
    }
};


class TextureUIElement: public UIElement {
protected:
    SDL_Texture* mTexture;
public:
    TextureUIElement(const float aLeft, const float aTop, const float aWidth, const float aHeight, SDL_Texture* aTexture)
        :   UIElement(aLeft, aTop, aWidth, aHeight),
            mTexture {aTexture} {}
    void Render(SDL_Renderer* aSDLRenderer);
    SDL_Texture* GetTexture() const
    {
        return mTexture;
    }
};


class SelectableUIElement: public TextureUIElement {
public:
    using TextureUIElement::TextureUIElement;
    bool IsSelected;
    void Render(SDL_Renderer* aSDLRenderer);
};