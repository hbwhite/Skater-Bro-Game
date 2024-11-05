//
//  PlayAdapter.h
//  chocolate
//
//  Created by Harrison White on 2/12/18.
//
//

#ifndef PlayAdapter_h
#define PlayAdapter_h

#ifdef SDKBOX_ENABLED

#include "pluginsdkboxplay/PluginSdkboxPlay.h"

using namespace sdkbox;

class PlayAdapter : public SdkboxPlayListener {
    /**
     * Call method invoked when the Plugin connection changes its status.
     * Values are as follows:
     *   + GPS_CONNECTED:       successfully connected.
     *   + GPS_DISCONNECTED:    successfully disconnected.
     *   + GPS_CONNECTION_ERROR:error with google play services connection.
     */
    void onConnectionStatusChanged( int status ) override {};
    
    /**
     * Callback method invoked when an score has been successfully submitted to a leaderboard.
     * It notifies back with the leaderboard_name (not id, see the sdkbox_config.json file) and the
     * subbmited score, as well as whether the score is the daily, weekly, or all time best score.
     * Since Game center can't determine if submitted score is maximum, it will send the max score flags as false.
     */
    void onScoreSubmitted( const std::string& leaderboard_name, long score, bool maxScoreAllTime, bool maxScoreWeek, bool maxScoreToday ) override {};
    
    /**
     * Callback method invoked from a call to `getMyScore` method.
     * `time_span` and `collection_type` are the supplied values to `getMyScore` method call.
     */
    void onMyScore( const std::string& leaderboard_name, int time_span, int collection_type, long score ) override { printf("******* asdlfkjaslkdfjlasjdf ****\n"); };
    
    /**
     * Callback method invoked from a call to `getMyScore` method and the method was errored.
     * `time_span` and `collection_type` are the supplied values to `getMyScore` method call.
     * `error_code` and `error_description` give extended info about the error.
     */
    void onMyScoreError( const std::string& leaderboard_name, int time_span, int collection_type, int error_code, const std::string& error_description) override { printf("my score error"); };
    
    /**
     * Callback method invoked from a call to `getPlayerCenteredScores` method.
     * `json_with_score_entries` is an json array enconded string, each of which elements is of the form:
     * Each json element contains the following information:
     * ```json
     *   {
     *      "display_rank"          : string,
     *      "display_score"         : string,
     *      "rank"                  : number,   // long
     *      "score"                 : number,   // long,
     *      "holder_display_name"   : string,
     *      "hires_imageuri"        : string,    // content:// protocol
     *      "lowres_imageuri"       : string,
     *      "tag"                   : string,
     *      "timestamp_millis"      : long
     *    }
     * ```
     * `time_span` and `collection_type` are the values supplied to `getPlayerCenteredScores` method.
     */
    void onPlayerCenteredScores( const std::string& leaderboard_name,
                                        int time_span,
                                        int collection_type,
                                        const std::string& json_with_score_entries ) override {};
    
    /**
     * Callback method invoked from a call to `getPlayerCenteredScores` method was errored.
     * `time_span` and `collection_type` are the values supplied to `getPlayerCenteredScores` method.
     * `error_code` and `error_description` give extended info about the error.
     */
    void onPlayerCenteredScoresError( const std::string& leaderboard_name,
                                             int time_span,
                                             int collection_type,
                                             int error_code,
                                             const std::string& error_description) override {};
    
    /**
     * Callback method invoked when the request call to increment an achievement is succeessful and
     * that achievement gets unlocked. This happens when the incremental step count reaches its maximum value.
     * Maximum step count for an incremental achievement is defined in the google play developer console.
     */
    void onIncrementalAchievementUnlocked( const std::string& achievement_name ) override {};
    
    /**
     * Callback method invoked when the request call to increment an achievement is successful.
     * If possible (Google play only) it notifies back with the current achievement step count.
     */
    void onIncrementalAchievementStep( const std::string& achievement_name, double step ) override {};
    
    void onIncrementalAchievementStepError( const std::string& name, double steps, int error_code, const std::string& error_description ) override {};
    
    /**
     * Call method invoked when the request call to unlock a non-incremental achievement is successful.
     * If this is the first time the achievement is unlocked, newUnlocked will be true.
     */
    void onAchievementUnlocked( const std::string& achievement_name, bool newlyUnlocked ) override {};
    
    void onAchievementUnlockError( const std::string& achievement_name, int error_code, const std::string& error_description ) override {};
    
    /**
     * Method invoked after calling plugin's `loadAchievements` method.
     * The `json_achievements_info` parameter is a json array encoded string.
     * #### Android fields:
     * each array element is of the form:
     * ```json
     *   {
     *      "id"                        : string,
     *      "name"                      : string,
     *      "xp_value"                  : string,   // experience value
     *      "last_updated_timestamp"    : number,
     *      "description"               : string,
     *      "type"                      : number,   // 0 = standard, 1 = incremental
     *      "state"                     : number,   // 0 = unlocked, 1 = revealed,   2 = hidden
     *      "unlocked_image_uri"        : string,   // content:// protocol
     *      "revealed_image_uri"        : string,   // content:// protocol
     *   }
     * ```
     *   If the achievement is incremental, these fileds will also be available:
     * ```json
     *   {
     *      "formatted_current_steps"   : string,
     *      "formatted_total_steps"     : string,
     *      current_steps"              : number,
     *      "total_steps"               : number
     *   }
     * ```
     * #### IOS fields:
     * ```json
     *   {
     *      "id"                        : string,
     *      "name"                      : string,
     *      "xp_value"                  : number, int
     *      "last_updated_timestamp"    : number,
     *      "description"               : string,   // maybe empty if no achievemnt submission happened before.
     *      "state"                     : number,   // 0 = unlocked, 1 = revealed,   2 = hidden
     *      "type"                      : 1,        // on ios all achievemtns are incremental.
     *      "current_steps"             : number,   // double value. percentage 0.0 .. 100.0
     *      "total_steps"               : number,   // 100.0
     *   }
     *  ```
     *  iOS only fields:
     * ```json
     *   {
     *      "replayable"                : boolean,
     *   }
     * ```
     */
    void onAchievementsLoaded( bool reload_forced, const std::string& json_achievements_info ) override {};
    
    void onSetSteps( const std::string& name, double steps ) override {};
    
    void onSetStepsError( const std::string& name, double steps, int error_code, const std::string& error_description ) override {};
    
    void onReveal( const std::string& name) override {};
    
    void onRevealError( const std::string& name, int error_code, const std::string& error_description ) override {};
    
    /**
     ****** DEPRECATED ******
     * @param action std::string save, load
     * @param name std::string
     * @param data std::string
     * @param error std::string if load/save success, error will be empty
     *
     */
    void onGameData(const std::string& action, const std::string& name, const std::string& data, const std::string& error) override {};
    
    /**
     * @param success bool
     * @param error std::string if success, error will be empty
     *
     */
    void onSaveGameData(bool success, const std::string& error) override {};
    
    /**
     * @param savedData SavedGameData*
     * @param error std::string if success, error will be empty
     *
     */
    void onLoadGameData(const SavedGameData* savedData, const std::string& error) override {};
};


#endif

#endif /* PlayAdapter_h */
