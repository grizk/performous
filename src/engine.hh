#include "audio.hh"
#include "pitch.hh"
#include "xtime.hh"
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <limits>
#include <list>
#include <utility>

struct Color { float r, g, b; };

struct Player {
	Analyzer& m_analyzer;
	Color m_color;
	typedef std::list<std::pair<double, double> > pitch_t;
	pitch_t m_pitch;
	double m_score;
	Player(Analyzer& analyzer): m_analyzer(analyzer), m_score() {}
	void prepare() { m_analyzer.process(); }
	void update() {
		Tone const* t = m_analyzer.findTone();
		if (t) m_pitch.push_back(std::make_pair(t->freq, t->stabledb));
		else m_pitch.push_back(std::make_pair(std::numeric_limits<double>::quiet_NaN(), -std::numeric_limits<double>::infinity()));
	}
	double score() const {
		return m_score;
	}

};

namespace {
	Color playerColors[] = {
		{ 52/255.0, 101/255.0, 164/255.0 },
		{ 1.0, 0.3, 0.3 }
	};
}

class Engine {
	CAudio& m_audio;
	std::list<Player> m_players;
	size_t m_time;
	volatile bool m_quit;
	boost::mutex m_mutex;
	boost::thread m_thread;  // IMPORTANT: This must be the last variable
  public:
	static const double TIMESTEP;
	/** Construct a new Engine with the players that go with it.
	* The engine runs in the background in a separate thread.
	* @param audio A reference will be stored in order to monitor playback time.
	* @param anBegin Analyzers to use (beginning iterator)
	* @param anBegin Analyzers to use (ending iterator)
	**/
	template <typename FwdIt> Engine(CAudio& audio, FwdIt anBegin, FwdIt anEnd):
	  m_audio(audio), m_players(anBegin, anEnd), m_time(), m_quit(), m_thread(boost::ref(*this))
	{
		size_t player = 0;
		for (std::list<Player>::iterator it = m_players.begin(); it != m_players.end(); ++it, ++player) it->m_color = playerColors[player]; // FIXME: don't segfault with more than two players
	}
	~Engine() { m_quit = true; m_thread.join(); }
	/** Used internally for boost::thread. Do not call this yourself. (boost::thread requires this to be public). **/
	void operator()() {
		while (!m_quit) {
			std::for_each(m_players.begin(), m_players.end(), boost::bind(&Player::prepare, _1));
			double t = m_audio.getPosition();
			double timeLeft = m_time * TIMESTEP - t;
			if (timeLeft > 0.0) { boost::thread::sleep(now() + std::min(TIMESTEP, timeLeft * 0.6)); continue; }
			boost::mutex::scoped_lock l(m_mutex);
			std::for_each(m_players.begin(), m_players.end(), boost::bind(&Player::update, _1));
			++m_time;
		}
	}
	std::list<Player> getPlayers() {
		boost::mutex::scoped_lock l(m_mutex);
		return m_players;
	}
};
