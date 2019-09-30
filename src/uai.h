#ifndef UAI_H_
#define UAI_H_

#define DEF_DEPTH 5
#define MAX_DEPTH 50

struct Settings {
    bool stop;

    int depth;
    int nodes;

    int wtime;
	int btime;
	int winc;
    int binc;

    int movetime;

    void init() {
        stop = false;

        depth = DEF_DEPTH;
        nodes = 0;

        wtime = 1000;
        btime = 1000;
        winc = 0;
        binc = 0;

        movetime = 0;
    }
};

extern Settings settings;

void uai();

void infoString(const int depth, const int score, const int nodes, const int duration, std::vector<Move> pv);

#endif // #ifndef UAI_H_