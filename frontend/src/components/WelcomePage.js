import React, { useState, useEffect } from 'react';
import { Button, Alert, Spinner } from 'react-bootstrap';
import { RiSpotifyFill, RiPlayFill, RiPauseFill, RiErrorWarningFill } from "react-icons/ri";
import './WelcomePage.css';

const WelcomePage = () => {
	const [isPlaying, setIsPlaying] = useState(false);
	const [currentTrack, setCurrentTrack] = useState(0);
	const [userData, setUserData] = useState(null);
	const [isLoading, setIsLoading] = useState(true);
	const [error, setError] = useState(null);

	// Compact featured tracks
	const featuredTracks = [
		{ title: "Summer Vibes", artist: "DJ Shurik", duration: "3:45" },
		{ title: "Night Drive", artist: "Electro Waves", duration: "4:20" },
		{ title: "Sunset Dreams", artist: "Mellow Beats", duration: "3:15" }
	];

	// Fetch user data with error handling
	useEffect(() => {
		const fetchUserData = async () => {
			try {
				setIsLoading(true);
				const response = await fetch('/music/');

				if (!response.ok) {
					throw new Error(`Server returned ${response.status}`);
				}

				const result = await response.json();
				setUserData(result);
				setError(null);
			} catch (err) {
				console.error('Failed to fetch user data:', err);
				setError(err.message);
				// Set default user data for demo purposes
				setUserData({ username: "Guest", auth: false });
			} finally {
				setIsLoading(false);
			}
		};

		fetchUserData();
	}, []);

	// Simulate audio playing with a timer
	useEffect(() => {
		let interval;
		if (isPlaying) {
			interval = setInterval(() => {
				setCurrentTrack(prev => (prev + 1) % featuredTracks.length);
			}, 3000); // Faster rotation for compact view
		}
		return () => clearInterval(interval);
	}, [isPlaying, featuredTracks.length]);

	const togglePlay = () => {
		setIsPlaying(!isPlaying);
	};

	const retryFetch = () => {
		setError(null);
		setIsLoading(true);
		// Retry the fetch after a short delay
		setTimeout(() => {
			window.location.reload();
		}, 500);
	};

	return (
		<div className="welcome-container">
			<div className="welcome-content-container">
				<div className="welcome-content">
					{/* Error message */}
					{error && (
						<Alert variant="danger" className="error-alert">
							<RiErrorWarningFill className="error-icon" />
							<div>
								<h6>Connection Issue</h6>
								<p>Showing demo content. Server unavailable.</p>
								<Button variant="outline-danger" size="sm" onClick={retryFetch}>
									Retry
								</Button>
							</div>
						</Alert>
					)}

					<div className="logo-section">
						<RiSpotifyFill className="logo-icon" />
						<h1 className="welcome-title">SHURIK <span className="accent">MUSIC</span></h1>
					</div>

					<p className="welcome-subtitle">
						Discover, stream, and share music from emerging and major artists worldwide.
					</p>

					<div className="user-welcome">
						{isLoading ? (
							<div className="user-loading">
								<Spinner animation="border" variant="light" size="sm" />
								<span>Checking status...</span>
							</div>
						) : userData && userData.auth ? (
							<p>Welcome back, <strong>{userData.username}</strong>!</p>
						) : (
							<p>Hello! Sign up to access personalized music experience.</p>
						)}
					</div>

					<div className="cta-buttons">
						<Button variant="success" size="lg" className="cta-button">
							Sign Up Free
						</Button>
						<Button variant="outline-light" size="lg" className="cta-button">
							Browse Music
						</Button>
					</div>

					<div className="featured-tracks">
						<h3>Featured Tracks</h3>
						<div className="tracks-list">
							{featuredTracks.map((track, index) => (
								<div
									key={index}
									className={`track-item ${index === currentTrack ? 'active' : ''}`}
								>
									<div className="track-info">
										<div className="track-title">{track.title}</div>
										<div className="track-artist">{track.artist}</div>
									</div>
									<div className="track-duration">{track.duration}</div>
								</div>
							))}
						</div>

						<div className="player-controls">
							<Button
								variant="outline-light"
								className="play-button"
								onClick={togglePlay}
								size="sm"
							>
								{isPlaying ? <RiPauseFill /> : <RiPlayFill />}
								{isPlaying ? 'Pause' : 'Play Demo'}
							</Button>
						</div>
					</div>

					<div className="stats-section">
						<div className="stat">
							<div className="stat-number">50K+</div>
							<div className="stat-label">Songs</div>
						</div>
						<div className="stat">
							<div className="stat-number">5K+</div>
							<div className="stat-label">Artists</div>
						</div>
						<div className="stat">
							<div className="stat-number">1M+</div>
							<div className="stat-label">Listeners</div>
						</div>
					</div>
				</div>
			</div>
		</div>
	);
};

export default WelcomePage;