import React from "react";
import { useState } from "react";
import NavBar from "./components/Navbar";
import ControlTable from "./components/ControlTable";

const App = () => {
  const [count, setCount] = useState(0);

  return (
    <div>
      <NavBar />
      <main className="flex flex-col items-center pt-32 py-2 w-[1068px] mx-auto h-screen">
        <ControlTable />
      </main>
    </div>
  );
};

export default App;
