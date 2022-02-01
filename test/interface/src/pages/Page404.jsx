import Tooltip from '@mui/material/Tooltip';
import { useReducer } from "react"
import { Link } from "react-router-dom"

import {ReactComponent as Worker} from 'assets/worker.svg'

export default () => {
    const navigate = () => {};


    return (
    <Tooltip title="Вернуться на главную">
        <Link 
            className="card d-flex align-items-center justify-content-center flex-column"
            to='/'
        >
            <Worker className="worker-icon pt-2 pl-2" style={{height: "50vh"}}/>
            <div
                className="header p-4"
            >
                Данной страницы не существует.
            </div>
        </Link>   
    </Tooltip>

    )
}