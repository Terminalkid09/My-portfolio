import discord
from discord.ext import commands
import whois

class WhoisLookup(commands.Cog):
    def __init__(self, bot):
        self.bot = bot

    @commands.command()
    async def whois(self, ctx, domain: str):
        """Mostra le informazioni WHOIS di un dominio"""

        await ctx.send(f"🔍 Recupero WHOIS per: `{domain}`")

        try:
            data = whois.whois(domain)
        except Exception as e:
            await ctx.send(f"❌ Errore durante la richiesta WHOIS: `{e}`")
            return

        embed = discord.Embed(
            title=f"🌐 WHOIS — {domain}",
            color=discord.Color.blue()
        )

        def safe(value):
            if isinstance(value, list):
                return ", ".join(str(v) for v in value)
            return value if value else "N/A"

        embed.add_field(name="📅 Creato", value=f"`{safe(data.creation_date)}`", inline=False)
        embed.add_field(name="⏳ Scadenza", value=f"`{safe(data.expiration_date)}`", inline=False)
        embed.add_field(name="🏢 Registrar", value=f"`{safe(data.registrar)}`", inline=False)
        embed.add_field(name="🧩 Nameserver", value=f"`{safe(data.name_servers)}`", inline=False)
        embed.add_field(name="📍 Paese", value=f"`{safe(data.country)}`", inline=False)
        embed.add_field(name="📧 Email", value=f"`{safe(data.emails)}`", inline=False)

        embed.set_footer(text="WHOIS Lookup")
        await ctx.send(embed=embed)

async def setup(bot):
    await bot.add_cog(WhoisLookup(bot))