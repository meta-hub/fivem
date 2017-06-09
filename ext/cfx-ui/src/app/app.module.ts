import {BrowserModule} from '@angular/platform-browser';
import {NgModule} from '@angular/core';
import {FormsModule} from '@angular/forms';
import {HttpModule} from '@angular/http';

import {NguiTabModule} from '@ngui/tab';
import {VirtualScrollModule} from 'angular2-virtual-scroll';
import {TranslationModule} from 'angular-l10n';
import {NouisliderModule} from 'ng2-nouislider';
import {MomentModule} from 'angular2-moment';
import {ContextMenuModule} from 'ngx-contextmenu';

import {AppRoutingModule} from './app-routing.module';
import {AppComponent} from './app.component';

import {AppNavComponent} from './app-nav.component';
import {ConnectingPopupComponent} from './servers/connecting-popup.component';
import {HomeComponent} from './home/home.component';
import {SettingsComponent} from './settings/settings.component';

import {ServerListComponent} from './servers/ui/server-list.component';
import {ServerListUiComponent} from './servers/ui/server-list-ui.component';
import {ServerListingComponent} from './servers/ui/server-listing.component';
import {ServerListingItemComponent} from './servers/ui/server-listing-item.component';
import {ServersTableHeaderComponent} from './servers/ui/servers-table-header.component';
import {ServerFilterComponent} from './servers/ui/server-filter.component';
import {DirectConnectComponent} from './servers/direct/direct-connect.component';

import {ServersService} from './servers/servers.service';
import {TweetService} from './home/tweet.service';

import {GameService, CfxGameService, DummyGameService} from './game.service';

import {ColorizePipe} from './colorize.pipe';
import {EscapePipe} from './escape.pipe';

@NgModule({
	declarations: [
		AppComponent,
		AppNavComponent,
		ConnectingPopupComponent,
		HomeComponent,
		SettingsComponent,
		ServerListComponent,
		ServerListUiComponent,
		ServerListingComponent,
		ServerListingItemComponent,
		ServersTableHeaderComponent,
		ServerFilterComponent,
		DirectConnectComponent,
		ColorizePipe,
		EscapePipe
	],
	imports:      [
		BrowserModule,
		FormsModule,
		HttpModule,
		AppRoutingModule,
		NguiTabModule,
		VirtualScrollModule,
		NouisliderModule,
		MomentModule,
		ContextMenuModule,
		TranslationModule.forRoot()
	],
	providers:    [
		ServersService,
		TweetService,
		{
			provide:  GameService,
			useClass: DummyGameService
		}
	],
	bootstrap:    [
		AppComponent
	]
})
export class AppModule {
}
