import React from "react";

export const NavBar = () => {
  return (
    <div className="w-screen">
      <nav className="bg-slate-800 fixed z-30 w-full p-4">
        <div className="flex items-center justify-start">
          <a href="/" className="text-xl font-bold flex items-center">
            <span className="self-center whitespace-nowrap text-white">
              Genie's lamp
            </span>
          </a>
        </div>
      </nav>
    </div>
  );
};

export default NavBar;
