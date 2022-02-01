import { useEffect, useMemo } from "react";
import {HashRouter as Router} from "react-router-dom"
import Main from 'main'
import Store from 'store'
import './style.scss'

import use$data from 'store/data'
import use$ui from 'store/ui'
import io from 'socket.io-client'


function Stored(){



   const page = useMemo(() => {
    return <Router>
      <Main/>   
    </Router>
  }, [])

  return page
}


export default function App(){


    return (
    <Store>
        <Stored/>
    </Store>
  );

}