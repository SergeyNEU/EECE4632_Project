void add_round_key(uint8_t state[4][4], uint8_t round_key[4][4])
{
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            state[row][col] ^= round_key[row][col];
        }
    }
}

void key_expansion(const uint8_t key[4 * 4], uint8_t round_keys[11][4][4])
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            round_keys[0][j][i] = key[j * 4 + i];
        }
    }
    for (int round = 1; round <= 10; ++round)
    {
        for (int i = 0; i < 4; ++i)
        {
            uint8_t temp[4];
            if (i == 0)
            {
                temp[0] = s_box[round_keys[round - 1][1][(i + 3) % 4]] ^ rcon[round];
                temp[1] = s_box[round_keys[round - 1][2][(i + 3) % 4]];
                temp[2] = s_box[round_keys[round - 1][3][(i + 3) % 4]];
                temp[3] = s_box[round_keys[round - 1][0][(i + 3) % 4]];
            }
            else
            {
                memcpy(temp, round_keys[round - 1][(i + 3) % 4], 4);
            }
            for (int j = 0; j < 4; ++j)
            {
                round_keys[round][i][j] = round_keys[round - 1][i][j] ^ temp[j];
            }
        }
    }
}