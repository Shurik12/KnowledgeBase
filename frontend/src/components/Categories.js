import React, { useState, useEffect } from "react";
import { Link, useNavigate } from 'react-router-dom';
import { ListGroup, Spinner, Alert, Button } from 'react-bootstrap';
import { RiPlayCircleFill, RiMore2Line } from "react-icons/ri";
import useApi from '../hooks/useApi';
import { useUser } from './context/UserContext';

const Categories = () => {
	const [categories, setCategories] = useState([]);
	const { callApi, loading, error, setError } = useApi();
	const navigate = useNavigate();
	const user = useUser(); // Get user from context

	useEffect(() => {
		// Redirect to login if not authenticated
		if (!user.auth) {
			navigate('/');
			return;
		}

		const fetchCategories = async () => {
			try {
				const result = await callApi("/music/categories");
				setCategories(result.categories || []);
			} catch (err) {
				console.error('Failed to fetch categories:', err);
			}
		};

		fetchCategories();
	}, [user.auth, navigate, callApi]);

	// Show loading or redirect if not authenticated
	if (!user.auth) {
		return (
			<div className="text-center p-4">
				<Spinner animation="border" />
				<div>Redirecting to login...</div>
			</div>
		);
	}

	if (loading) return (
		<div className="text-center p-4">
			<Spinner animation="border" />
			<div>Loading categories...</div>
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
		<div className="categories-container">
			<h3 className="text-center mb-4">Categories</h3>

			<ListGroup variant="flush">
				{categories.map(category => (
					<ListGroup.Item
						key={category.name}
						className="category-item d-flex align-items-center"
					>
						<Button variant="link" className="p-0 me-3">
							<RiPlayCircleFill size={20} />
						</Button>

						<Link
							to={`/category/${category.name}`}
							className="text-success text-decoration-none flex-grow-1"
						>
							{category.name}
						</Link>

						<Button variant="link" className="p-0">
							<RiMore2Line size={20} />
						</Button>
					</ListGroup.Item>
				))}
			</ListGroup>

			{categories.length === 0 && !loading && (
				<div className="text-center p-4">
					<p>No categories found</p>
					<Button variant="primary" onClick={() => window.location.reload()}>
						Refresh
					</Button>
				</div>
			)}
		</div>
	);
};

export default Categories;