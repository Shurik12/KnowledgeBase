import { useState, useEffect } from "react";
import { useParams } from 'react-router-dom';
import { ListGroup, Button, Spinner, Alert, Badge } from 'react-bootstrap';
import { RiHeartFill, RiHeartLine, RiDeleteBin3Line } from "react-icons/ri";
import ReactPlayer from "react-player";

const Author = () => {
	const { author } = useParams();
	const [authorData, setAuthorData] = useState({});
	const [username, setUsername] = useState("");
	const [isLoading, setIsLoading] = useState(true);
	const [error, setError] = useState(null);

	useEffect(() => {
		const fetchAuthor = async () => {
			try {
				setIsLoading(true);
				const response = await fetch(`/music/authors/${author}`);
				if (!response.ok) throw new Error('Failed to fetch author');

				const result = await response.json();
				setAuthorData(result.author || {});
				setUsername(result.username || "");
			} catch (err) {
				setError(err.message);
			} finally {
				setIsLoading(false);
			}
		};

		fetchAuthor();
	}, [author]);

	const handleLike = async () => {
		try {
			await fetch("/music/like_author", {
				method: "POST",
				headers: {
					'Content-Type': 'application/json',
				},
				body: JSON.stringify(authorData)
			});

			setAuthorData(prev => ({
				...prev,
				likes: prev.likes.includes(username)
					? prev.likes.filter(user => user !== username)
					: [...prev.likes, username]
			}));
		} catch (error) {
			console.error('Error liking author:', error);
		}
	};

	if (isLoading) return (
		<div className="text-center p-4">
			<Spinner animation="border" />
			<div>Loading author...</div>
		</div>
	);

	if (error) return (
		<Alert variant="danger" className="m-3">
			Error: {error}
		</Alert>
	);

	const isLiked = authorData.likes?.includes(username);

	return (
		<div className="author-container">
			<div className="author-header text-center mb-4">
				<h3>{authorData.name} ({authorData.year})</h3>
			</div>

			<ListGroup variant="flush">
				{authorData.tracks?.map((track, index) => (
					<ListGroup.Item key={index} className="author-track">
						<div className="d-flex align-items-center justify-content-between">
							<div className="track-info">
								<div className="track-name">{track}</div>
								<ReactPlayer
									url={`/mediafiles/${authorData.name} - ${track}.mp3`}
									width="300px"
									height="40px"
									playing={false}
									controls={true}
								/>
							</div>

							<div className="track-actions">
								<Button variant="link" className="text-danger">
									<RiDeleteBin3Line />
								</Button>
							</div>
						</div>
					</ListGroup.Item>
				))}
			</ListGroup>

			<div className="author-footer mt-3">
				<Badge bg="primary" className="like-badge">
					<Button
						variant="link"
						onClick={handleLike}
						className={`p-0 ${isLiked ? 'text-danger' : 'text-white'}`}
					>
						{isLiked ? <RiHeartFill /> : <RiHeartLine />}
					</Button>
					{authorData.likes?.length || 0}
				</Badge>
			</div>
		</div>
	);
};

export default Author;