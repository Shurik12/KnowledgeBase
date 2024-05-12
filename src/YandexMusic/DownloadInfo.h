#pragma once

#include <string>
#include <vector>
 
using namespace std;

class DownloadInfo
{

public:

	DownloadInfo()= default;

	DownloadInfo(
		string  downloadInfoUrl_,
		string  bitrateInKbps_);

	~DownloadInfo()= default;

    /// Get private members of class
	[[nodiscard]] string getDownloadInfoUrl() const;
	[[nodiscard]] string getBitrateInKbps() const;

    /// Set private members of class
	void setDownloadInfoUrl(const string& value);
	void setBitrateInKbps(const string& value);

private:

	string downloadInfoUrl;
	string bitrateInKbps;
	string downloadTrackUrl;

};