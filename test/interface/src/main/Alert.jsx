import Alert from '@mui/material/Alert';

import CloseIcon from '@mui/icons-material/Close'
import IconButton from '@mui/material/IconButton';

import use$ui from 'store/ui'
import { useEffect, useReducer, useState } from 'react';

export default function TopAlert(){
    const [{alertText, alertType, alertCloseOn},{closeAlert}] = use$ui()

    const [, forseUpdate] = useReducer(()=>new Date(),0)

    const shouldShow = !!alertText && (alertCloseOn > new Date() || alertCloseOn < 0)

    useEffect(() => {
        if(shouldShow && alertCloseOn > 0)
        {
            const delay = alertCloseOn - new Date()
            setTimeout(() => forseUpdate(), delay > 0 ? delay : 0)
        }
    })

    return (
  
        <Alert 
            key={"top-alert"}
            className={"top-alert" + (shouldShow ? " visible-top-alert" : '')}
            severity={alertType||'info'}
            action={
                <IconButton
                    aria-label="close"
                    color="inherit"
                    size="small"
                    onClick={closeAlert}
                    >
                        <CloseIcon fontSize="inherit" />
                </IconButton>
            }
        >
            {alertText || ''}
            
        </Alert>
    )
}