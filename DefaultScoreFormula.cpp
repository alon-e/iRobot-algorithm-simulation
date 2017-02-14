#include <algorithm>
#include <string>
#include <map>

using namespace std;

bool IsParameterMissing(const map<string, int>& score_params, const string& parameterName) {
	return score_params.find(parameterName) == score_params.end();
}
		
extern "C" int calc_score(const map<string, int>& score_params)
{
	if (IsParameterMissing(score_params, "actual_position_in_competition")) return -1;
	if (IsParameterMissing(score_params, "winner_num_steps")) return -1;
	if (IsParameterMissing(score_params, "this_num_steps")) return -1;
	if (IsParameterMissing(score_params, "sum_dirt_in_house")) return -1;
	if (IsParameterMissing(score_params, "dirt_collected")) return -1;
	if (IsParameterMissing(score_params, "is_back_in_docking")) return -1;

	int position, afterWinner, dirtLeft, dockScore;

	position = score_params.at("actual_position_in_competition");
	afterWinner = score_params.at("winner_num_steps") - score_params.at("this_num_steps");
	dirtLeft = score_params.at("sum_dirt_in_house") - score_params.at("dirt_collected");
	dockScore = score_params.at("is_back_in_docking") == 1 ? 50 : -200;

	//robot finished successfully
	if (dirtLeft == 0 && dockScore == 50) position = min(position, 4);
	else position = 10;

	int score = 2000 - (position - 1) * 50 + afterWinner * 10 - dirtLeft * 3 + dockScore;
	return max(0, score);
}