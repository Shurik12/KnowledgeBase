import React, { useState, useEffect } from "react";
import { useParams } from 'react-router-dom';
import { ListGroup, Spinner, Alert, Button } from 'react-bootstrap';
import { ListSong } from './ListSong';
import useApi from '../hooks/useApi';

const Profile = () => {
	const { username } = useParams();
	const [profileData, setProfileData] = useState({});
	const { callApi, loading, error, setError } = useApi();

	useEffect(() => {
		const fetchProfile = async () => {
			try {
				const result = await callApi(`/music/profiles/${username}`);
				setProfileData(result);
			} catch (err) {
				// Error is already set by useApi
			}
		};

		fetchProfile();
	}, [username, callApi]);

	if (loading) return (
		<div className="text-center p-4">
			<Spinner animation="border" />
			<div>Loading profile...</div>
		</div>
	);

	if (error) return (
		<Alert variant="danger" className="m-3">
			Error: {error}
			<Button variant="outline-danger" size="sm" className="ms-2" onClick={() => setError(null)}>
				Dismiss
			</Button>
		</Alert>
	);

	return (
		<div className="profile-container">
			<div className="profile-header text-center">
				<h3>{username}</h3>
			</div>

			<ListGroup className="profile-content">
				<ListGroup.Item>
					<h5>Liked Artists</h5>
					<div className="authors-list">
						{profileData.authors?.join(", ") || "No liked artists yet"}
					</div>
				</ListGroup.Item>

				<ListGroup.Item>
					<h5>Liked Music</h5>
					<ListSong data={profileData} />
				</ListGroup.Item>
			</ListGroup>
		</div>
	);
};

export default Profile;