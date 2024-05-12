#include <YandexMusic/DownloadInfo.h>

#include <utility>

DownloadInfo::DownloadInfo(
	string  downloadInfoUrl_,
	string  bitrateInKbps_)
	: downloadInfoUrl(std::move(downloadInfoUrl_)),
	bitrateInKbps(std::move(bitrateInKbps_))
{}


string DownloadInfo::getDownloadInfoUrl() const
{
	return downloadInfoUrl;
}


string DownloadInfo::getBitrateInKbps() const
{
	return bitrateInKbps;
}


void DownloadInfo::setDownloadInfoUrl(const string& value)
{
	downloadInfoUrl = value;
}


void DownloadInfo::setBitrateInKbps(const string& value)
{
	bitrateInKbps = value;
}