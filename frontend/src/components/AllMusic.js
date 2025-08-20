// AllMusic.js - Updated with better error handling
import React, { useState, useEffect } from "react";
import { Spinner, Alert, Button } from 'react-bootstrap';
import { ListSong } from './ListSong';

const AllMusic = () => {
	const [musicData, setMusicData] = useState({});
	const [isLoading, setIsLoading] = useState(true);
	const [error, setError] = useState(null);

	const fetchMusic = async () => {
		try {
			setIsLoading(true);
			setError(null);

			const response = await fetch("/music/shurik_music", {
				credentials: 'include'
			});

			if (!response.ok) {
				throw new Error(`Failed to fetch music: ${response.status}`);
			}

			const result = await response.json();
			setMusicData(result);
		} catch (err) {
			setError(err.message);
			// Set demo data for offline mode
			setMusicData({
				tracks: [
					{ id: 1, name: "Summer Vibes", author: "DJ Shurik", like: [], url: "/demo/summer-vibes.mp3" },
					{ id: 2, name: "Night Drive", author: "Electro Waves", like: [], url: "/demo/night-drive.mp3" }
				]
			});
		} finally {
			setIsLoading(false);
		}
	};

	useEffect(() => {
		fetchMusic();
	}, []);

	if (isLoading) return (
		<div className="text-center p-4">
			<Spinner animation="border" />
			<div>Loading music...</div>
		</div>
	);

	return (
		<div className="all-music-container">
			<h3 className="text-center mb-4">All Tracks</h3>

			{error && (
				<Alert variant="warning" className="mb-3">
					{error} - Showing demo data
					<Button variant="outline-warning" size="sm" className="ms-2" onClick={fetchMusic}>
						Retry
					</Button>
				</Alert>
			)}

			<ListSong data={musicData} />
		</div>
	);
};

export default AllMusic;