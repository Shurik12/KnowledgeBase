import React, { useState, useEffect } from "react";
import { BrowserRouter as Router, Routes, Route, Link, useNavigate } from 'react-router-dom';
import {
	Navbar,
	Nav,
	Form,
	FormControl,
	InputGroup,
	Button,
	Container,
	Spinner,
	Alert,
	Modal // Added Modal import
} from 'react-bootstrap';
import { BsSearch, BsX } from "react-icons/bs"; // Added BsX for close icon

import AllMusic from './AllMusic';
import WelcomePage from './WelcomePage';
import Categories from './Categories';
import Category from './Category';
import Author from './Author';
import Profile from './Profile';
import { UserProvider, useUser } from './context/UserContext';
import './App.css';

const App = () => {
	const [user, setUser] = useState({
		username: "",
		auth: false,
		message: ""
	});
	const [isLoading, setIsLoading] = useState(true);
	const [error, setError] = useState(null);
	const [showLoginModal, setShowLoginModal] = useState(false); // Added modal state
	const [showSignupModal, setShowSignupModal] = useState(false); // Added modal state

	useEffect(() => {
		// Try to load user data from localStorage first for faster UI
		const savedUser = localStorage.getItem('userAuth');
		if (savedUser) {
			try {
				const parsedUser = JSON.parse(savedUser);
				setUser(parsedUser);
			} catch (e) {
				console.warn('Failed to parse saved user data:', e);
			}
		}

		fetchUserData();
	}, []);

	const fetchUserData = async () => {
		try {
			// Add timeout to prevent hanging requests
			const controller = new AbortController();
			const timeoutId = setTimeout(() => controller.abort(), 8000);

			const response = await fetch('/music/', {
				signal: controller.signal,
				credentials: 'include' // Important for session cookies
			});

			clearTimeout(timeoutId);

			if (!response.ok) {
				if (response.status === 401) {
					// Session expired, clear local storage
					localStorage.removeItem('userAuth');
					throw new Error('Session expired');
				}
				throw new Error(`Server error: ${response.status} ${response.statusText}`);
			}

			const result = await response.json();
			setUser(result);
			// Store in localStorage for persistence during refreshes
			localStorage.setItem('userAuth', JSON.stringify(result));
			setError(null);
		} catch (err) {
			console.warn('Failed to fetch user data:', err.message);

			// Use cached data if available, otherwise use guest mode
			const cachedUser = localStorage.getItem('userAuth');
			if (cachedUser && err.name !== 'AbortError') {
				try {
					const parsedUser = JSON.parse(cachedUser);
					setUser(parsedUser);
					setError(`Connection issue: ${err.message}. Using cached session.`);
				} catch (parseError) {
					setUser({
						username: "Guest",
						auth: false,
						message: "Using offline mode"
					});
					setError(`Connection issue: ${err.message}. Using demo mode.`);
				}
			} else if (err.name !== 'AbortError') {
				setUser({
					username: "Guest",
					auth: false,
					message: "Using offline mode"
				});
				setError(`Connection issue: ${err.message}. Using demo mode.`);
			} else {
				setError('Request timed out. Using demo mode.');
			}
		} finally {
			setIsLoading(false);
		}
	};

	const handleAuth = async (url, data) => {
		try {
			setIsLoading(true);
			const response = await fetch(url, {
				method: "POST",
				headers: {
					'Content-Type': 'application/json',
				},
				body: JSON.stringify(data),
				credentials: 'include' // Important for session cookies
			});

			if (!response.ok) {
				throw new Error(`Authentication failed: ${response.status}`);
			}

			const result = await response.json();

			if (result.auth) {
				// Store successful authentication in localStorage
				localStorage.setItem('userAuth', JSON.stringify(result));
			} else {
				// Clear any stale auth data on failure
				localStorage.removeItem('userAuth');
			}

			setUser(result);
			setError(null);
		} catch (err) {
			setError(err.message);
			localStorage.removeItem('userAuth');
		} finally {
			setIsLoading(false);
		}
	};

	const handleLogin = async (event) => {
		event.preventDefault();
		const formData = new FormData(event.target);
		const data = {
			username: formData.get('username'),
			password: formData.get('password')
		};

		await handleAuth('/music/login', data);
		setShowLoginModal(false); // Close modal after login
	};

	const handleSignup = async (event) => {
		event.preventDefault();
		const formData = new FormData(event.target);
		const data = {
			username: formData.get('username'),
			password: formData.get('password'),
			confirmation: formData.get('confirm_password'),
			email: formData.get('email')
		};

		await handleAuth('/music/register', data);
		setShowSignupModal(false); // Close modal after signup
	};

	const handleLogout = async () => {
		try {
			await fetch('/music/logout', {
				method: 'POST',
				credentials: 'include'
			});
		} catch (err) {
			console.warn('Logout API call failed:', err);
		} finally {
			// Always clear local state and storage
			setUser({ username: "", auth: false, message: "" });
			localStorage.removeItem('userAuth');
			setError(null);
		}
	};

	// Add session refresh mechanism
	useEffect(() => {
		if (user.auth) {
			const refreshInterval = setInterval(() => {
				fetchUserData().catch(err => {
					console.log('Session refresh failed:', err);
				});
			}, 5 * 60 * 1000); // Refresh every 5 minutes

			return () => clearInterval(refreshInterval);
		}
	}, [user.auth]);

	if (isLoading) {
		return (
			<div className="loading-container">
				<Spinner animation="border" />
				<div className="mt-2">Loading...</div>
			</div>
		);
	}

	return (
		<UserProvider user={user}>
			<Router>
				<div className="app">
					<Navbar bg="dark" variant="dark" expand="lg" className="navbar-custom">
						<Container fluid className="px-3">
							<Navbar.Brand as={Link} to="/" className="me-0 me-md-3">
								<img
									alt="Shurik Music"
									src="https://i.pinimg.com/originals/c8/eb/91/c8eb914f7ba2ffc48a4369c893b1f43f.gif"
									width="120"
									height="40"
									className="d-inline-block align-top"
								/>
							</Navbar.Brand>

							<Navbar.Toggle aria-controls="basic-navbar-nav" />

							<Navbar.Collapse id="basic-navbar-nav">
								{user.auth ? (
									<>
										<Form className="d-flex me-auto my-2 my-lg-0">
											<InputGroup>
												<FormControl
													placeholder="Search music..."
													aria-label="Search"
													className="search-input"
												/>
												<Button variant="outline-light">
													<BsSearch />
												</Button>
											</InputGroup>
										</Form>

										<Nav className="ms-auto">
											<Nav.Link as={Link} to="/music/shurik_music" className="px-2 px-md-3">
												Music
											</Nav.Link>
											<Nav.Link as={Link} to="/categories" className="px-2 px-md-3">
												Categories
											</Nav.Link>
											<Nav.Link as={Link} to={`/profiles/${user.username}`} className="px-2 px-md-3">
												Profile
											</Nav.Link>
											<Nav.Link onClick={handleLogout} className="px-2 px-md-3">
												Logout
											</Nav.Link>
										</Nav>
									</>
								) : (
									<Nav className="ms-auto">
										<Button
											variant="outline-light"
											className="me-2"
											onClick={() => setShowLoginModal(true)}
										>
											<i className="bi bi-box-arrow-in-right me-1"></i>
											Sign in
										</Button>
										<Button
											variant="success"
											onClick={() => setShowSignupModal(true)}
										>
											<i className="bi bi-person-plus me-1"></i>
											Sign up
										</Button>
									</Nav>
								)}
							</Navbar.Collapse>
						</Container>
					</Navbar>

					{/* Login Modal */}
					<Modal
						show={showLoginModal}
						onHide={() => setShowLoginModal(false)}
						centered
						dialogClassName="modal-fullscreen"
					>
						<Modal.Body className="auth-modal-body">
							<div className="auth-modal-content">
								<button
									className="auth-close-btn"
									onClick={() => setShowLoginModal(false)}
								>
									<BsX size={28} />
								</button>
								<LoginForm
									onSubmit={handleLogin}
									message={user.message}
									onClose={() => setShowLoginModal(false)}
								/>
							</div>
						</Modal.Body>
					</Modal>

					{/* Signup Modal */}
					<Modal
						show={showSignupModal}
						onHide={() => setShowSignupModal(false)}
						centered
						dialogClassName="modal-fullscreen"
					>
						<Modal.Body className="auth-modal-body">
							<div className="auth-modal-content">
								<button
									className="auth-close-btn"
									onClick={() => setShowSignupModal(false)}
								>
									<BsX size={28} />
								</button>
								<SignupForm
									onSubmit={handleSignup}
									message={user.message}
									onClose={() => setShowSignupModal(false)}
								/>
							</div>
						</Modal.Body>
					</Modal>

					<main className="main-content">
						<Container fluid className="px-3 px-md-4">
							{error && (
								<Alert variant="warning" className="mt-3 mb-0" dismissible onClose={() => setError(null)}>
									{error}
								</Alert>
							)}

							<Routes>
								<Route path="/" element={<div className="welcome-route"><WelcomePageWrapper /></div>} />
								<Route path="/music/shurik_music" element={<div className="route-container"><AuthWrapper><AllMusic /></AuthWrapper></div>} />
								<Route path="/authors/:author" element={<div className="route-container"><AuthWrapper><Author /></AuthWrapper></div>} />
								<Route path="/profiles/:username" element={<div className="route-container"><AuthWrapper><Profile /></AuthWrapper></div>} />
								<Route path="/categories" element={<div className="route-container"><AuthWrapper><Categories /></AuthWrapper></div>} />
								<Route path="/category/:category" element={<div className="route-container"><AuthWrapper><Category /></AuthWrapper></div>} />
							</Routes>
						</Container>
					</main>
				</div>
			</Router>
		</UserProvider>
	);
};

// Update LoginForm component
const LoginForm = ({ onSubmit, message, onClose }) => {
	const [isLoading, setIsLoading] = useState(false);

	const handleSubmit = async (event) => {
		event.preventDefault();
		setIsLoading(true);
		await onSubmit(event);
		setIsLoading(false);
	};

	return (
		<div className="auth-form-container">
			<div className="auth-form-header">
				<h2 className="auth-form-title">
					<i className="bi bi-music-note-beamed me-2"></i>
					Welcome Back
				</h2>
				<p className="auth-form-subtitle">Sign in to your Shurik Music account</p>
			</div>

			<Form onSubmit={handleSubmit} className="auth-form">
				<Form.Group className="mb-3">
					<Form.Label className="auth-form-label">Username</Form.Label>
					<Form.Control
						type="text"
						name="username"
						placeholder="Enter your username"
						required
						className="auth-form-input"
						disabled={isLoading}
					/>
				</Form.Group>

				<Form.Group className="mb-4">
					<Form.Label className="auth-form-label">Password</Form.Label>
					<Form.Control
						type="password"
						name="password"
						placeholder="Enter your password"
						required
						className="auth-form-input"
						disabled={isLoading}
					/>
				</Form.Group>

				{message && (
					<div className="auth-form-error">
						<i className="bi bi-exclamation-triangle me-2"></i>
						{message}
					</div>
				)}

				<Button
					type="submit"
					variant="primary"
					className="auth-form-button"
					disabled={isLoading}
					size="lg"
				>
					{isLoading ? (
						<>
							<Spinner animation="border" size="sm" className="me-2" />
							Signing in...
						</>
					) : (
						<>
							<i className="bi bi-box-arrow-in-right me-2"></i>
							Sign In
						</>
					)}
				</Button>

				<div className="auth-form-footer">
					<p className="auth-form-help">
						<i className="bi bi-info-circle me-1"></i>
						Demo account: <strong>user</strong> / <strong>password</strong>
					</p>
				</div>
			</Form>
		</div>
	);
};

// Update SignupForm component
const SignupForm = ({ onSubmit, message, onClose }) => {
	const [isLoading, setIsLoading] = useState(false);

	const handleSubmit = async (event) => {
		event.preventDefault();
		setIsLoading(true);
		await onSubmit(event);
		setIsLoading(false);
	};

	return (
		<div className="auth-form-container">
			<div className="auth-form-header">
				<h2 className="auth-form-title">
					<i className="bi bi-person-plus me-2"></i>
					Join Shurik Music
				</h2>
				<p className="auth-form-subtitle">Create your account to start listening</p>
			</div>

			<Form onSubmit={handleSubmit} className="auth-form">
				<Form.Group className="mb-3">
					<Form.Label className="auth-form-label">Username</Form.Label>
					<Form.Control
						type="text"
						name="username"
						placeholder="Choose a username"
						required
						className="auth-form-input"
						disabled={isLoading}
					/>
				</Form.Group>

				<Form.Group className="mb-3">
					<Form.Label className="auth-form-label">Email</Form.Label>
					<Form.Control
						type="email"
						name="email"
						placeholder="Enter your email"
						required
						className="auth-form-input"
						disabled={isLoading}
					/>
				</Form.Group>

				<Form.Group className="mb-3">
					<Form.Label className="auth-form-label">Password</Form.Label>
					<Form.Control
						type="password"
						name="password"
						placeholder="Create a password"
						required
						className="auth-form-input"
						disabled={isLoading}
					/>
				</Form.Group>

				<Form.Group className="mb-4">
					<Form.Label className="auth-form-label">Confirm Password</Form.Label>
					<Form.Control
						type="password"
						name="confirm_password"
						placeholder="Confirm your password"
						required
						className="auth-form-input"
						disabled={isLoading}
					/>
				</Form.Group>

				{message && (
					<div className="auth-form-error">
						<i className="bi bi-exclamation-triangle me-2"></i>
						{message}
					</div>
				)}

				<Button
					type="submit"
					variant="success"
					className="auth-form-button"
					disabled={isLoading}
					size="lg"
				>
					{isLoading ? (
						<>
							<Spinner animation="border" size="sm" className="me-2" />
							Creating Account...
						</>
					) : (
						<>
							<i className="bi bi-person-plus me-2"></i>
							Create Account
						</>
					)}
				</Button>

				<div className="auth-form-footer">
					<p className="auth-form-help">
						<i className="bi bi-shield-check me-1"></i>
						Your personal data is securely encrypted
					</p>
				</div>
			</Form>
		</div>
	);
};

// Update AuthWrapper to use the context
const AuthWrapper = ({ children }) => {
	const navigate = useNavigate();
	const user = useUser(); // Get user from context

	useEffect(() => {
		if (!user.auth) {
			navigate('/');
		}
	}, [user.auth, navigate]);

	if (!user.auth) {
		return (
			<div className="text-center p-4">
				<Spinner animation="border" />
				<div>Redirecting to login...</div>
			</div>
		);
	}

	return children;
};

// Update WelcomePageWrapper to use the context
const WelcomePageWrapper = () => {
	const navigate = useNavigate();
	const user = useUser(); // Get user from context

	useEffect(() => {
		if (user.auth) {
			navigate('/music/shurik_music');
		}
	}, [user.auth, navigate]);

	return <WelcomePage />;
};

export default App;