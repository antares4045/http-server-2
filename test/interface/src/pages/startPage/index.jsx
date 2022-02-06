import React, {useCallback, useEffect, useReducer, useState, useRef} from 'react'


import io from 'socket.io-client'
import axios from 'axios'
import { Button } from '@mui/material'
import links from 'constants/links'

export default function StartPage(){


        return <div width="100vw" height="100vh" className='pageRoot'>
            <Button
                  onClick={
                        async () => {
                              await axios({
                              cors : false,
                              method : 'post',
                              url : links.functionResolver,
                              data : {
                                    "header": {
                                        "function": "F1"
                                    },
                                    "body": { "lst" : [
                                        1,
                                        2,
                                        3
                                    ]}

                                }
                        })
                  }}
            >
                  send F1      
            </Button>    
            <Button
                  onClick={
                        async () => {
                              await axios({
                              cors : false,
                              method : 'post',
                              url : links.functionResolver,
                              data : {
                                    "header": {
                                        "function": "F2"
                                    },
                                    "body": { "lst" : [
                                        3,
                                        2,
                                        1
                                    ]}

                                }
                        })
                  }}
            >
                  send F2      
            </Button>    
        </div>
}