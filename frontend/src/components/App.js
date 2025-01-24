import React, { Component } from "react";
import ReactDOM from "react-dom/client";
import { BrowserRouter as HashRouter, Link, Route, Routes } from 'react-router-dom'
import { Nav, Navbar, NavDropdown, InputGroup, FormControl, Button, ListGroup, Form } from 'react-bootstrap';
import { BsSearch } from "react-icons/bs";

import AllMusic from './AllMusic';
import WelcomePage from './WelcomePage';
import Categories from './Categories';
import Category from './Category';
import Author from './Author';
import Profile from './Profile';


class App extends Component {

	constructor(props) {
		super(props);
		this.state = {
			username: "",
			auth: false,
			message: "",
			isFetching: false,
			error: null
		};
		this.handleFetch = this.handleFetch.bind(this);
		this.handleSubmitLogin = this.handleSubmitLogin.bind(this);
		this.handleSubmitSignUp = this.handleSubmitSignUp.bind(this);
		this.handleSubmitLogout = this.handleSubmitLogout.bind(this);
	}

	handleFetch(url, data) {
		fetch(url, { method: "post", body: JSON.stringify(data) })
			.then(response => response.json())
			.then(result => this.setState({
				username: result["username"],
				auth: result["auth"],
				message: result["message"],
				isFetching: false
			}))
			.catch(e => {
				console.log(e);
				this.setState({
					// username: result["username"],
					// auth: result["auth"],
					// message: result["message"],
					// isFetching: false,
					// error: e
					username: "username",
					auth: "auth",
					message: "message",
					isFetching: false
				});
			});
	}

	handleSubmitLogin(event) {

		const data = {
			"username": event.target.username.value,
			"password": event.target.password.value
		};
		event.target.username.value = "";
		event.target.password.value = "";
		this.handleFetch('/music/login', data);
		event.preventDefault();
	}

	handleSubmitSignUp(event) {
		const data = {
			"username": event.target.username.value,
			"password": event.target.password.value,
			"confirmation": event.target.confirm_password.value,
			"email": event.target.email.value
		};
		this.handleFetch('/music/register', data);
		event.preventDefault();
	}

	handleSubmitLogout(event) {
		this.setState({
			username: "",
			auth: false,
			message: "",
			isFetching: false
		});
		this.handleFetch('/music/logout');
		event.preventDefault();
	}

	refreshPage() {
		window.location.reload(false);
	}

	componentDidMount() {
		this.handleFetch('/music/');
	}

	render() {

		if (this.state.isFetching) return <div>...Loading</div>;
		if (this.state.error) return <div>{`Error: ${e.message}`}</div>;

		return (
			<HashRouter>
				<Navbar bg="dark" variant="dark" style={{ height: "10vh" }}>
					<Navbar.Brand href="#">
						<Link className="navbar-brand mr-0 mr-md-2" to="/">
							<img
								alt=""
								src="https://i.pinimg.com/originals/c8/eb/91/c8eb914f7ba2ffc48a4369c893b1f43f.gif"
								width="120"
								height="40"
								className="d-inline-block align-top"
							/>
						</Link>
					</Navbar.Brand>
					<Navbar.Toggle aria-controls="basic-navbar-nav" />
					{this.state.auth
						?
						<Navbar.Collapse id="basic-navbar-nav">
							<Form inline>
								<InputGroup>
									<FormControl
										placeholder="Search"
										aria-label="Search"
										aria-describedby="basic-addon1"
									/>
									<Button variant="outline-secondary"><BsSearch /></Button>
								</InputGroup>
							</Form>
							<Nav className="mr-auto">
								<Nav.Link>
									<Link to="/music/shurik_music" className="navbar-nav nav-item nav-link">Music </Link>
								</Nav.Link>
								<Nav.Link>
									<Link to="/categories" className="navbar-nav nav-item nav-link">Categories </Link>
								</Nav.Link>
								<Nav.Link>
									<Link className="navbar-nav nav-item nav-link" to={`/profiles/${this.state.username}`}>Profile </Link>
								</Nav.Link>
								<NavDropdown title="Dropdown" id="basic-nav-dropdown" className="navbar-nav nav-item nav-link">
									<NavDropdown.Item href="#action/3.1">Action</NavDropdown.Item>
									<NavDropdown.Item href="#action/3.2">Another action</NavDropdown.Item>
									<NavDropdown.Item href="#action/3.3">Something</NavDropdown.Item>
									<NavDropdown.Divider />
									<NavDropdown.Item href="#action/3.4">Separated link</NavDropdown.Item>
								</NavDropdown>
							</Nav>
							<Nav>
								<Nav.Link className="navbar-nav" onClick={this.handleSubmitLogout}>
									<Link className="navbar-nav nav-item nav-link" to={'/'}>Log out </Link>
								</Nav.Link>
							</Nav>
						</Navbar.Collapse>
						:
						<Navbar.Collapse id="basic-navbar-nav">
							<Nav>
								<NavDropdown title="Sign in" alignRight>
									<Form onSubmit={this.handleSubmitLogin}>
										<NavDropdown.Header>Sign in with your social media account</NavDropdown.Header>
										<ListGroup horizontal>
											<Button className="ml-2 btn btn-primary col-sm">Facebook</Button>
											<Button className="mx-2 btn btn-success col-sm">Twitter</Button>
										</ListGroup>
										<NavDropdown.Divider />
										<div className="form-group mx-2">
											<input type="text" name="username" className="form-control" placeholder="Username" required="required" />
										</div>
										<div className="form-group mx-2">
											<input type="password" name="password" className="form-control" placeholder="Password" required="required" />
										</div>
										{
											this.state.message && <div className="text-center text-danger">{this.state.message}</div>
										}
										<div className="mx-2">
											<input type="submit" className="btn btn-primary btn-sm btn-block" value="Login" />
										</div>
										<div className="text-center mt-2">
											<Link to="#">Forgot Your password?</Link>
										</div>
									</Form>
								</NavDropdown>
								<NavDropdown title="Sign up" alignRight>
									<Form onSubmit={this.handleSubmitSignUp}>
										<NavDropdown.Header>Please fill in this form to create an account!</NavDropdown.Header>
										<div className="form-group mx-2">
											<input type="text" className="form-control" name="username" placeholder="Username" required="required" />
										</div>
										<div className="form-group mx-2">
											<input type="email" className="form-control" name="email" placeholder="Email Address" required="required" />
										</div>
										<div className="form-group mx-2">
											<input type="text" className="form-control" name="password" placeholder="Password" required="required" />
										</div>
										<div className="form-group mx-2">
											<input type="text" className="form-control" name="confirm_password" placeholder="Confirm Password" required="required" />
										</div>
										{
											this.state.message && <div className="text-center text-danger">{this.state.message}</div>
										}
										<div className="form-group mx-2">
											<label className="form-check-label">
												<input type="checkbox" required="required" /> I accept the
												<a href="#">Terms of Use &amp; Privacy Policy</a>
											</label>
										</div>
										<div className="form-group mx-2">
											<button type="submit" className="btn btn-success btn-sm btn-block">Sign Up</button>
										</div>
									</Form>
									<div className="text-center">Already have an account? <a href="#">Login here</a></div>
								</NavDropdown>
							</Nav>
						</Navbar.Collapse>
					}
				</Navbar>

				<Routes>
					<Route exact path='/' Component={WelcomePage} />
					<Route path='/music/shurik_music' Component={AllMusic} />
					<Route path='/authors/:author' Component={Author} />
					<Route path='/profiles/:username' Component={Profile} />
					<Route path='/categories' Component={Categories} />
					<Route path='/category/:category' Component={Category} />
				</Routes>
			</HashRouter>
		);
	}
}

export default App;

const root = ReactDOM.createRoot(document.getElementById("app"));
root.render(
	<React.StrictMode>
		<App />
	</React.StrictMode>
);