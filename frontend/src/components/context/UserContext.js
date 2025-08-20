import React, { createContext, useContext } from 'react';

const UserContext = createContext();

export const useUser = () => {
	const context = useContext(UserContext);
	if (!context) {
		throw new Error('useUser must be used within a UserProvider');
	}
	return context;
};

export const UserProvider = ({ children, user }) => {
	return (
		<UserContext.Provider value={user}>
			{children}
		</UserContext.Provider>
	);
};

export default UserContext;