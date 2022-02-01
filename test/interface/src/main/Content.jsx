import React, { useMemo, useState } from "react";

import {Switch} from "react-router-dom"
import { Suspense, lazy } from "react"

import Loader from 'components/Spinner'

import use$ui from 'store/ui'
import { Redirect } from "react-router-dom/cjs/react-router-dom.min";

const LazyTab = ({tab, ...props}) => {
    const Component = lazy(() => import(`pages/${tab.page}`))
    return (
        <Component {...props}/>
    )
}



export default function Content(){
    
    const [{tabs, openingTab},{openTab},{availTabs : getAvailTabs}] = use$ui()

    const availTabs = useMemo(() => {
        return getAvailTabs()
    }, [tabs])

    const pageSwitch = useMemo(() => <Suspense fallback={<Loader/>}>
        <Switch>
            {availTabs.map(tab => <LazyTab path={tab.route} exact={tab.exactRoute} key={tab.id} tab={tab}/>)}
        </Switch>
    </Suspense>, [availTabs])

    if(openingTab){
        const tab = tabs.find(tab => tab.id == openingTab)
        
        openTab(null)
        if(tab)
            return <Redirect to={tab.route}/>
    }

    return pageSwitch
}