import React, { useState, useEffect } from "react";
import { useParams } from 'react-router-dom';
import { Spinner, Alert } from 'react-bootstrap';
import { ListSong } from './ListSong';

const Category = () => {
	const { category } = useParams();
	const [categoryData, setCategoryData] = useState({});
	const [isLoading, setIsLoading] = useState(true);
	const [error, setError] = useState(null);

	useEffect(() => {
		const fetchCategory = async () => {
			try {
				setIsLoading(true);
				const response = await fetch(`/music/category/${category}`);
				if (!response.ok) throw new Error('Failed to fetch category');

				const result = await response.json();
				setCategoryData(result);
			} catch (err) {
				setError(err.message);
			} finally {
				setIsLoading(false);
			}
		};

		fetchCategory();
	}, [category]);

	if (isLoading) return (
		<div className="text-center p-4">
			<Spinner animation="border" />
			<div>Loading category...</div>
		</div>
	);

	if (error) return (
		<Alert variant="danger" className="m-3">
			Error: {error}
		</Alert>
	);

	return (
		<div className="category-container">
			<h3 className="text-center category-title">
				{category.charAt(0).toUpperCase() + category.slice(1)}
			</h3>
			<ListSong data={categoryData} />
		</div>
	);
};

export default Category;