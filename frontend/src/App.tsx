import React from "react";
import { useState } from "react";
import NavBar from "./components/Navbar";
import ControlTable from "./components/ControlTable";
import useToast from "./hooks/useToasts";

const App = () => {
  const { toasts } = useToast();

  return (
    <div>
      <NavBar />
      <main className="flex flex-col items-center pt-32 py-2 w-[1068px] mx-auto h-screen">
        <ControlTable />
      </main>
      {toasts}
    </div>
  );
};

export default App;
