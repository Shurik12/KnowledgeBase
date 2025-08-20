import React, { useState } from 'react';
import { ListGroup, Button } from 'react-bootstrap';
import {
  RiPauseCircleFill,
  RiPlayCircleFill,
  RiMore2Line,
  RiHeartFill,
  RiHeartLine,
  RiDeleteBin3Line
} from "react-icons/ri";
import { Link } from 'react-router-dom';
import ReactPlayer from "react-player";

const Song = ({ track, user, onLike, onDelete }) => {
  const [isPlaying, setIsPlaying] = useState(false);
  const [isLiked, setIsLiked] = useState(track.like.includes(user));

  const handleLike = async () => {
    try {
      await fetch("/music/like_track", {
        method: "POST",
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(track)
      });
      setIsLiked(!isLiked);
      onLike?.(track.id, !isLiked);
    } catch (error) {
      console.error('Error liking track:', error);
    }
  };

  const handlePlayPause = () => {
    setIsPlaying(!isPlaying);
  };

  return (
    <ListGroup horizontal className="song-item">
      <ListGroup.Item className="song-controls">
        <Button
          variant="link"
          onClick={handlePlayPause}
          className="p-0"
        >
          {isPlaying ? <RiPauseCircleFill /> : <RiPlayCircleFill />}
        </Button>
      </ListGroup.Item>

      <ListGroup.Item className="song-info flex-grow-1">
        <div className="song-title">{track.name}</div>
        <Link
          to={`/authors/${track.author}`}
          className="text-success song-author"
        >
          {track.author}
        </Link>
      </ListGroup.Item>

      <ListGroup.Item className="song-actions">
        <Button
          variant="link"
          onClick={handleLike}
          className={`like-btn ${isLiked ? 'liked' : ''}`}
        >
          {isLiked ? <RiHeartFill /> : <RiHeartLine />}
        </Button>

        <Button variant="link" onClick={() => onDelete?.(track.id)}>
          <RiDeleteBin3Line />
        </Button>

        <Button variant="link">
          <RiMore2Line />
        </Button>
      </ListGroup.Item>

      {isPlaying && (
        <div className="audio-player">
          <ReactPlayer
            url={track.url || `/mediafiles/${track.author} - ${track.name}.mp3`}
            width="100%"
            height="40px"
            playing={isPlaying}
            controls={true}
          />
        </div>
      )}
    </ListGroup>
  );
};

export default React.memo(Song);