#pragma once

#include <string>
#include <utility>

namespace YandexMusic
{
	class DownloadInfo
	{
	public:
		DownloadInfo() = default;

		DownloadInfo(std::string downloadInfoUrl, std::string bitrateInKbps)
			: downloadInfoUrl_(std::move(downloadInfoUrl)), bitrateInKbps_(std::move(bitrateInKbps))
		{
		}

		[[nodiscard]] const std::string &downloadInfoUrl() const noexcept { return downloadInfoUrl_; }
		[[nodiscard]] const std::string &bitrateInKbps() const noexcept { return bitrateInKbps_; }

		void setDownloadInfoUrl(std::string url) { downloadInfoUrl_ = std::move(url); }
		void setBitrateInKbps(std::string bitrate) { bitrateInKbps_ = std::move(bitrate); }

	private:
		std::string downloadInfoUrl_;
		std::string bitrateInKbps_;
	};
} // namespace YandexMusic