# pragma once
# include <SDL3/SDL.h>

class FpsCounter {
private:
	Uint64 mLastTime;
	int mFrameCount = 0;
	float mElapsedTime = 0.f;
	float mDeltaTime = 0.f;
	float mFps = 0.f;
public:
	FpsCounter() : mLastTime{ SDL_GetTicks() } {};
	bool Update()
	{
		Uint64 currentTime = SDL_GetTicks();
		mDeltaTime = (currentTime - mLastTime) / 1000.f;
		mLastTime = currentTime;
		mElapsedTime += mDeltaTime;
		mFrameCount++;
		if (mElapsedTime >= 1.f)
		{
			mFps = mFrameCount / mElapsedTime;
			mElapsedTime = 0.f;
			mFrameCount = 0;
			return true;
		}
		return false;
	};
	const float GetFps() const { return mFps; };
	const float GetDeltaTime() const { return mDeltaTime; };
};