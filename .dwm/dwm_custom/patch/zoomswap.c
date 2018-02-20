static Client * findbefore(Client *c);

Client *
findbefore(Client *c) {
	Client *tmp;
	if(c == selmon->clients)
		return NULL;
	for(tmp = selmon->clients; tmp && tmp->next != c; tmp = tmp->next) ;
	return tmp;
}
