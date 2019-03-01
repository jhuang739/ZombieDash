#ifndef LEVEL_H_
#define LEVEL_H_

#include "GameConstants.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

class Level
{
public:

    enum MazeEntry {
        empty, player, dumb_zombie, smart_zombie, citizen, wall, exit, pit,
        vaccine_goodie, gas_can_goodie, landmine_goodie
    };

    enum LoadResult {
        load_success, load_fail_file_not_found, load_fail_bad_format
    };

    Level(std::string assetPath)
     : m_assetPath(assetPath)
    {
        for (int y = 0; y < LEVEL_HEIGHT; y++)
            for (int x = 0; x < LEVEL_WIDTH; x++)
                m_maze[y][x] = empty;
    }

    LoadResult loadLevel(std::string filename)
    {
        std::ifstream levelFile((m_assetPath + filename).c_str());
        if (!levelFile)
            return load_fail_file_not_found;

          // get the maze

        std::string line;
        bool foundExit = false;
        bool foundPlayer = false;

        for (int y = LEVEL_HEIGHT-1; std::getline(levelFile, line); y--)
        {
            if (y < 0)    // too many maze lines?
            {
                if (line.find_first_not_of(" \t\r") != std::string::npos)
                    return load_fail_bad_format;  // non-blank line

                char dummy;
                if (levelFile >> dummy)     // non-blank rest of file
                    return load_fail_bad_format;
                break;
            }

            if (line.size() < LEVEL_WIDTH  ||  line.find_first_not_of(" \t\r", LEVEL_WIDTH) != std::string::npos)
                return load_fail_bad_format;
                
            for (int x = 0; x < LEVEL_WIDTH; x++)
            {
                MazeEntry& me = m_maze[y][x];
                switch (toupper(line[x]))
                {
                    default:   return load_fail_bad_format;
                    case ' ':  me = empty;                      break;
                    case 'X':  me = exit;   foundExit = true;   break;
                    case '@':  me = player; foundPlayer = true; break;
                    case 'D':  me = dumb_zombie;                break;
                    case 'S':  me = smart_zombie;               break;
                    case 'C':  me = citizen;                    break;
                    case '#':  me = wall;                       break;
                    case 'O':  me = pit;                        break;
                    case 'V':  me = vaccine_goodie;             break;
                    case 'G':  me = gas_can_goodie;             break;
                    case 'L':  me = landmine_goodie;            break;
                }
            }
        }

        if (!foundExit  ||  !foundPlayer  ||  !edgesValid())
            return load_fail_bad_format;

        return load_success;
    }

    MazeEntry getContentsOf(int x, int y) const
    {
        return (x >= 0 && x < LEVEL_WIDTH && y >= 0 && y < LEVEL_HEIGHT) ? m_maze[y][x] : empty;
    }

private:
    MazeEntry   m_maze[LEVEL_HEIGHT][LEVEL_WIDTH];
    std::string m_assetPath;

    bool edgesValid() const
    {
        for (int y = 0; y < LEVEL_HEIGHT; y++)
            if (m_maze[y][0] != wall || m_maze[y][LEVEL_WIDTH-1] != wall)
                return false;
        for (int x = 0; x < LEVEL_WIDTH; x++)
            if (m_maze[0][x] != wall || m_maze[LEVEL_HEIGHT-1][x] != wall)
                return false;

        return true;
    }
};

#endif // LEVEL_H_
