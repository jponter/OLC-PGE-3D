/*

Animation helper class
(c) James Ponter

usage:



define in your main cpp:

olc::cAnimator animPlayer;

use the helper function to take sprite from a single png file

animPlayer.hLoadSprite("idle", "idle right 4.png" , 4, 32, 32);

or pushback frames individually

eg.
animPlayer.mapStates["idle"].push_back(new olc::Sprite("idle_1.png",));
animPlayer.mapStates["idle"].push_back(new olc::Sprite("idle_2.png";




*/


namespace olc
{



class cAnimator :	public olc::PGEX
{
public:
	std::map<std::string, std::vector<olc::Sprite*>> mapStates;

	std::string sCurrentState;
	int nCurrentFrame = 0;
	float fTimeBetweenFrames = 0.1f;
	float fTimeCounter = 0.0f;

	void ChangeState(std::string s)
	{
		if (s != sCurrentState)
		{
			sCurrentState = s;
			fTimeCounter = 0;
			nCurrentFrame = 0;
		}
	}

	void Update(float fElapsedTime)
	{
		fTimeCounter += fElapsedTime;
		if (fTimeCounter >= fTimeBetweenFrames)
		{
			fTimeCounter -= fTimeBetweenFrames;
			nCurrentFrame++;
			if (nCurrentFrame >= mapStates[sCurrentState].size())
				nCurrentFrame = 0;
		}
	}

	void DrawSelf(olc::PixelGameEngine* pge, olc::GFX2D::Transform2D& t)
	{
		olc::GFX2D::DrawSprite(mapStates[sCurrentState][nCurrentFrame], t);
	}

	void hLoadSprite(std::string state, std::string sprfile, int frames, int h, int w) {

		olc::Sprite* tmpMainSprite = new olc::Sprite(sprfile);
		for (int i = 0; i < frames; i++) {
			olc::Sprite* tmpSprite = new olc::Sprite(w, h);
			pge->SetPixelMode(olc::Pixel::MASK);
			pge->SetDrawTarget(tmpSprite);
			pge->Clear(olc::BLANK);

			pge->DrawPartialSprite(0, 0, tmpMainSprite, i * w, 0, w, h);
			this->mapStates[state].push_back(tmpSprite);

		}
		pge->SetDrawTarget(nullptr);
		pge->SetPixelMode(olc::Pixel::NORMAL);


	}

};

}
