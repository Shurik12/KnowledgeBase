import { useState, useCallback } from 'react';

export const useApi = () => {
	const [loading, setLoading] = useState(false);
	const [error, setError] = useState(null);

	const callApi = useCallback(async (url, options = {}) => {
		setLoading(true);
		setError(null);

		try {
			const config = {
				credentials: 'include',
				headers: {
					'Content-Type': 'application/json',
					'Cache-Control': 'no-cache, no-store, must-revalidate',
					'Pragma': 'no-cache',
					...options.headers
				},
				...options
			};

			const response = await fetch(url, config);

			// Handle unauthorized responses
			if (response.status === 401) {
				// Clear any stale authentication data
				localStorage.removeItem('userAuth');
				throw new Error('Session expired. Please login again.');
			}

			if (!response.ok) {
				throw new Error(`API error: ${response.status} ${response.statusText}`);
			}

			const data = await response.json();
			return data;
		} catch (err) {
			setError(err.message);
			throw err;
		} finally {
			setLoading(false);
		}
	}, []);

	const clearError = useCallback(() => {
		setError(null);
	}, []);

	return { callApi, loading, error, clearError, setError };
};

export default useApi;